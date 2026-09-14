#ifndef NAVIGATION_SERVICE_H
#define NAVIGATION_SERVICE_H
#include <QObject>
#include <QString>
#include <QList>

// One driving maneuver from the AMap route response.
struct NavigationStep {
    QString instruction;   // human readable turn instruction
    QString roadName;      // road name; may be empty
    int distanceMeters;    // step distance in meters
    QString polyline;      // "lng,lat;lng,lat;..." in GCJ-02
    NavigationStep() : distanceMeters(0) {}
};

// Real AMap driving route (v5 with show_fields=cost, or v3 fallback fields).
struct NavigationRoute {
    int totalDistanceMeters;
    int totalDurationSeconds;
    QList<NavigationStep> steps;
    NavigationRoute() : totalDistanceMeters(0), totalDurationSeconds(0) {}
    bool isValid() const { return totalDistanceMeters > 0 && !steps.isEmpty(); }
};

class QNetworkAccessManager;
class QNetworkReply;
class QTimer;

// Signal-driven AMap Web Service route client with an explicit state machine.
//
// States:  Idle --requestRoute()--> Requesting --ok--> Ready
//          Requesting --error/timeout/api failure--> Failed
//          Ready/Failed --requestRoute()--> Requesting again
//
// All requests are asynchronous (QNetworkAccessManager + finished signal) and
// never block the UI thread. A successful (origin,destination) answer is
// cached for cache_ttl_seconds so demo replay and page refreshes do not burn
// the personal daily quota (about 100 calls/day for driving directions).
//
// Config: /Kd1234/config/navigation.ini (see config/navigation.ini.example).
// Errors are classified so the UI can show a specific reason per state.
// Qt 5.4.1 compatible: no QNetworkReply::errorOccurred (checked via error()
// inside the finished handler); no QSslError::errorOccurred.
class NavigationService : public QObject {
    Q_OBJECT
public:
    enum State { Idle, Requesting, Ready, Failed };
    enum ErrorKind {
        NoError = 0,
        ConfigError,   // missing key or unreadable ini
        NetworkError,  // connection / DNS / HTTP transport failure
        SslError,      // TLS handshake failure
        ApiError,      // AMap returned status != 1 (invalid key, quota, ...)
        ParseError,    // JSON ok but structure unexpected
        NoRouteError,  // status 1 but no paths
        TimeoutError   // request timed out
    };

    // configPath empty => board default /Kd1234/config/navigation.ini.
    explicit NavigationService(const QString &configPath = QString(), QObject *parent = 0);

    State state() const { return m_state; }
    ErrorKind lastError() const { return m_lastError; }
    QString lastErrorText() const { return m_lastErrorText; }
    NavigationRoute route() const { return m_route; }
    bool configured() const { return !m_key.isEmpty(); }

    // Async route query. Coordinates are "lng,lat" in GCJ-02.
    // Emits routeReady() or routeFailed(). A cached pair inside the TTL
    // re-emits routeReady() without touching the network.
    void requestRoute(const QString &originLngLat, const QString &destinationLngLat);

    // Uses origin/destination from the ini (location_mode=fixed first version).
    void requestConfiguredRoute();

    // Pure response parser, no network. Exposed for unit tests and fixtures.
    // Accepts both v5 (cost.duration + show_fields) and v3 (path.duration)
    // field layouts. Returns false and fills errorOut on any problem.
    static bool parseRouteResponse(const QByteArray &json, NavigationRoute *out, QString *errorOut);

signals:
    void routeReady(const NavigationRoute &route);
    void routeFailed(int errorKind, const QString &errorText);

private slots:
    void onReplyFinished();

private:
    void fail(ErrorKind kind, const QString &text);
    void loadConfig(const QString &configPath);
    static int intFromVariant(const class QJsonValue &value, int fallback = 0);

    QNetworkAccessManager *m_manager;
    QNetworkReply *m_reply;
    QTimer *m_timeout;

    // configuration
    QString m_key;
    QString m_origin, m_destination;
    int m_strategy;
    QString m_showFields;
    bool m_useHttps;
    int m_timeoutMs;
    int m_cacheTtlSeconds;

    // request/cache bookkeeping
    QString m_pendingOrigin, m_pendingDestination;
    QString m_cacheKey;
    NavigationRoute m_cacheRoute;
    qint64 m_cacheTimestamp;
    bool m_timedOut;

    State m_state;
    ErrorKind m_lastError;
    QString m_lastErrorText;
    NavigationRoute m_route;
};
#endif
