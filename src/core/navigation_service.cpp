#include "navigation_service.h"
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSettings>
#include <QSslCertificate>
#include <QSslConfiguration>
#include <QTimer>
#include <QUrl>
#include <QUrlQuery>
#include <QVariant>

namespace {
const char kDefaultConfigPath[] = "/Kd1234/config/navigation.ini";
const char kDrivingEndpointHttps[] = "https://restapi.amap.com/v5/direction/driving";
const char kDrivingEndpointHttpV3[] = "http://restapi.amap.com/v3/direction/driving"; // HTTPS-unavailable fallback
const char kBoardCaBundle[] = "/etc/ssl/certs/ca-certificates.crt";

QString coordinatePairKey(const QString &a, const QString &b) {
    return a.trimmed() + QStringLiteral("->") + b.trimmed();
}
}

NavigationService::NavigationService(const QString &configPath, QObject *parent)
    : QObject(parent)
    , m_manager(new QNetworkAccessManager(this))
    , m_reply(0)
    , m_timeout(new QTimer(this))
    , m_strategy(0)
    , m_useHttps(true)
    , m_timeoutMs(8000)
    , m_cacheTtlSeconds(600)
    , m_cacheTimestamp(0)
    , m_timedOut(false)
    , m_state(Idle)
    , m_lastError(NoError)
{
    m_timeout->setSingleShot(true);
    connect(m_timeout, &QTimer::timeout, this, [this]() {
        if (m_reply) {
            m_timedOut = true;
            m_reply->abort();
        }
    });
    loadConfig(configPath.isEmpty() ? QString::fromLatin1(kDefaultConfigPath) : configPath);
}

// QSettings IniFormat splits values containing commas into QStringList
// (legacy behaviour), which would silently empty out "origin=lng,lat",
// "destination=lng,lat" and "show_fields=a,b,c". Read back either form.
static QString settingsText(const QSettings &settings, const QString &key,
                            const QString &fallback = QString()) {
    const QVariant v = settings.value(key);
    if (!v.isValid())
        return fallback;
    if (v.type() == QVariant::StringList) {
        const QString joined = v.toStringList().join(QStringLiteral(","));
        return joined.isEmpty() ? fallback : joined;
    }
    const QString text = v.toString();
    return text.isEmpty() ? fallback : text;
}

void NavigationService::loadConfig(const QString &configPath) {
    m_key.clear();
    m_origin.clear();
    m_destination.clear();
    if (!QFileInfo(configPath).isReadable())
        return; // stay unconfigured; UI keeps demo content
    QSettings settings(configPath, QSettings::IniFormat);
    settings.beginGroup(QStringLiteral("amap"));
    m_key = settingsText(settings, QStringLiteral("web_service_key")).trimmed();
    m_origin = settingsText(settings, QStringLiteral("origin")).trimmed();
    m_destination = settingsText(settings, QStringLiteral("destination")).trimmed();
    m_strategy = settings.value(QStringLiteral("strategy"), 0).toInt();
    m_showFields = settingsText(settings, QStringLiteral("show_fields"),
                                QStringLiteral("cost,tmcs,navi,roads")).trimmed();
    m_useHttps = settings.value(QStringLiteral("use_https"), true).toBool();
    m_timeoutMs = settings.value(QStringLiteral("request_timeout_ms"), 8000).toInt();
    m_cacheTtlSeconds = settings.value(QStringLiteral("cache_ttl_seconds"), 600).toInt();
    settings.endGroup();
}

void NavigationService::requestConfiguredRoute() {
    if (m_origin.isEmpty() || m_destination.isEmpty()) {
        fail(ConfigError, QStringLiteral("navigation.ini has no origin/destination"));
        return;
    }
    requestRoute(m_origin, m_destination);
}

void NavigationService::requestRoute(const QString &originLngLat, const QString &destinationLngLat) {
    if (m_key.isEmpty()) {
        fail(ConfigError, QStringLiteral("AMap web_service_key missing in %1")
                                 .arg(QString::fromLatin1(kDefaultConfigPath)));
        return;
    }
    const QString pairKey = coordinatePairKey(originLngLat, destinationLngLat);
    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    if (m_cacheRoute.isValid() && m_cacheKey == pairKey
        && (now - m_cacheTimestamp) < qint64(m_cacheTtlSeconds) * 1000) {
        // Cache hit: same pair inside TTL, no network call.
        m_route = m_cacheRoute;
        m_state = Ready;
        m_lastError = NoError;
        m_lastErrorText.clear();
        emit routeReady(m_route);
        return;
    }

    if (m_reply) { // a request is already in flight; ignore re-entry
        return;
    }

    m_pendingOrigin = originLngLat.trimmed();
    m_pendingDestination = destinationLngLat.trimmed();
    m_state = Requesting;
    m_timedOut = false;

    QUrl url(QString::fromLatin1(m_useHttps ? kDrivingEndpointHttps : kDrivingEndpointHttpV3));
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("key"), m_key);
    query.addQueryItem(QStringLiteral("origin"), m_pendingOrigin);
    query.addQueryItem(QStringLiteral("destination"), m_pendingDestination);
    query.addQueryItem(QStringLiteral("strategy"), QString::number(m_strategy));
    if (m_useHttps && !m_showFields.isEmpty())
        query.addQueryItem(QStringLiteral("show_fields"), m_showFields);
    url.setQuery(query);

    QNetworkRequest request(url);
#ifdef Q_OS_LINUX
    // Board ships a CA bundle; load it per-request like tools/navigation_https_probe.
    const QList<QSslCertificate> certificates =
        QSslCertificate::fromPath(QString::fromLatin1(kBoardCaBundle), QSsl::Pem);
    if (!certificates.isEmpty()) {
        QSslConfiguration ssl = QSslConfiguration::defaultConfiguration();
        ssl.setCaCertificates(certificates);
        request.setSslConfiguration(ssl);
    }
#endif
    request.setRawHeader("User-Agent", "NEV-SmartHMI/0.2");

    m_reply = m_manager->get(request);
    connect(m_reply, &QNetworkReply::finished, this, &NavigationService::onReplyFinished);
    m_timeout->start(m_timeoutMs);
}

void NavigationService::onReplyFinished() {
    if (!m_reply)
        return;
    QNetworkReply *reply = m_reply;
    m_reply = 0;
    m_timeout->stop();
    reply->deleteLater();

    if (m_timedOut) {
        fail(TimeoutError, QStringLiteral("route request timed out after %1 ms").arg(m_timeoutMs));
        return;
    }
    if (reply->error() == QNetworkReply::SslHandshakeFailedError) {
        fail(SslError, reply->errorString());
        return;
    }
    if (reply->error() != QNetworkReply::NoError) {
        fail(NetworkError, reply->errorString());
        return;
    }

    const QByteArray body = reply->readAll();
#ifdef Q_OS_LINUX
    // Keep the last raw response on the board for field-name inspection.
    QFile dump(QStringLiteral("/Kd1234/config/navigation_last_response.json"));
    if (dump.open(QIODevice::WriteOnly | QIODevice::Truncate))
        dump.write(body);
#endif
    NavigationRoute parsed;
    QString parseError;
    if (!parseRouteResponse(body, &parsed, &parseError)) {
        fail(parseError == QStringLiteral("noroute") ? NoRouteError : ParseError, parseError);
        return;
    }

    m_route = parsed;
    m_cacheRoute = parsed;
    m_cacheKey = coordinatePairKey(m_pendingOrigin, m_pendingDestination);
    m_cacheTimestamp = QDateTime::currentMSecsSinceEpoch();
    m_state = Ready;
    m_lastError = NoError;
    m_lastErrorText.clear();
    emit routeReady(m_route);
}

void NavigationService::fail(ErrorKind kind, const QString &text) {
    m_state = Failed;
    m_lastError = kind;
    m_lastErrorText = text;
    emit routeFailed(static_cast<int>(kind), text);
}

int NavigationService::intFromVariant(const QJsonValue &value, int fallback) {
    const QVariant variant = value.toVariant();
    bool ok = false;
    const int result = variant.toInt(&ok);
    return ok ? result : fallback;
}

bool NavigationService::parseRouteResponse(const QByteArray &json, NavigationRoute *out, QString *errorOut) {
    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(json, &parseError);
    if (document.isNull()) {
        if (errorOut) *errorOut = QStringLiteral("invalid JSON: %1").arg(parseError.errorString());
        return false;
    }
    const QJsonObject root = document.object();
    const QString status = root.value(QStringLiteral("status")).toString();
    if (status != QStringLiteral("1")) {
        const QString infocode = root.value(QStringLiteral("infocode")).toString();
        const QString info = root.value(QStringLiteral("info")).toString();
        if (errorOut)
            *errorOut = QStringLiteral("AMap API error %1: %2").arg(infocode, info);
        return false;
    }
    const QJsonObject routeObject = root.value(QStringLiteral("route")).toObject();
    const QJsonArray paths = routeObject.value(QStringLiteral("paths")).toArray();
    if (paths.isEmpty()) {
        if (errorOut) *errorOut = QStringLiteral("noroute");
        return false;
    }
    const QJsonObject path = paths.at(0).toObject();

    NavigationRoute result;
    result.totalDistanceMeters = intFromVariant(path.value(QStringLiteral("distance")));
    // v5 puts duration under cost (needs show_fields=cost); v3 keeps it on the path.
    result.totalDurationSeconds = intFromVariant(path.value(QStringLiteral("duration")));
    if (result.totalDurationSeconds <= 0) {
        const QJsonObject cost = path.value(QStringLiteral("cost")).toObject();
        result.totalDurationSeconds = intFromVariant(cost.value(QStringLiteral("duration")));
    }

    const QJsonArray steps = path.value(QStringLiteral("steps")).toArray();
    for (int i = 0; i < steps.size(); ++i) {
        const QJsonObject stepObject = steps.at(i).toObject();
        NavigationStep step;
        step.instruction = stepObject.value(QStringLiteral("instruction")).toString();
        // v3 names it "road"; some v5 payloads expose "road_name" via show_fields=roads.
        step.roadName = stepObject.value(QStringLiteral("road")).toString();
        if (step.roadName.isEmpty())
            step.roadName = stepObject.value(QStringLiteral("road_name")).toString();
        step.distanceMeters = intFromVariant(stepObject.value(QStringLiteral("distance")));
        if (step.distanceMeters <= 0)
            // v5 names the per-step distance "step_distance" (v3 uses "distance").
            step.distanceMeters = intFromVariant(stepObject.value(QStringLiteral("step_distance")));
        step.polyline = stepObject.value(QStringLiteral("polyline")).toString();
        if (step.polyline.isEmpty()) {
            // v5 has no per-step "polyline"; the geometry lives in
            // tmcs[].tmc_polyline (traffic segments) when show_fields=tmcs.
            QStringList merged;
            const QJsonArray tmcs = stepObject.value(QStringLiteral("tmcs")).toArray();
            for (int t = 0; t < tmcs.size(); ++t) {
                const QString segment = tmcs.at(t).toObject()
                    .value(QStringLiteral("tmc_polyline")).toString();
                if (!segment.isEmpty())
                    merged << segment;
            }
            step.polyline = merged.join(QLatin1Char(';'));
        }
        result.steps.append(step);
    }

    if (result.totalDistanceMeters <= 0 || result.steps.isEmpty()) {
        if (errorOut) *errorOut = QStringLiteral("route structure incomplete: distance=%1 steps=%2")
                                       .arg(result.totalDistanceMeters).arg(result.steps.size());
        return false;
    }
    if (out) *out = result;
    return true;
}
