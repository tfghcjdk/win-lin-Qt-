#include "phone_location_receiver.h"
#include <QDateTime>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUdpSocket>
#include <QTimer>

PhoneLocationReceiver::PhoneLocationReceiver(QObject *parent)
    : QObject(parent)
    , m_socket(0)
    , m_staleTimer(new QTimer(this))
    , m_maxAccuracy(50.0f)
    , m_staleTimeoutMs(5000)
    , m_accepted(0)
    , m_rejected(0)
    , m_stale(true)
{
    m_staleTimer->setSingleShot(true);
    connect(m_staleTimer, &QTimer::timeout, this, &PhoneLocationReceiver::onStaleCheck);
}

PhoneLocationReceiver::~PhoneLocationReceiver() {
    stop();
}

bool PhoneLocationReceiver::start(quint16 port) {
    stop();
    m_socket = new QUdpSocket(this);
    if (!m_socket->bind(QHostAddress::Any, port, QUdpSocket::ShareAddress)) {
        emit receiverError(QStringLiteral("UDP bind :%1 failed: %2")
                           .arg(port).arg(m_socket->errorString()));
        delete m_socket;
        m_socket = 0;
        return false;
    }
    connect(m_socket, &QUdpSocket::readyRead, this, &PhoneLocationReceiver::onReadyRead);
    m_stale = true;
    return true;
}

void PhoneLocationReceiver::stop() {
    if (m_socket) {
        m_socket->close();
        m_socket->deleteLater();
        m_socket = 0;
    }
    m_staleTimer->stop();
}

bool PhoneLocationReceiver::listening() const {
    return m_socket && m_socket->state() == QAbstractSocket::BoundState;
}

void PhoneLocationReceiver::setStaleTimeoutMs(int ms) {
    m_staleTimeoutMs = qMax(1000, ms);
}

void PhoneLocationReceiver::onReadyRead() {
    while (m_socket && m_socket->hasPendingDatagrams()) {
        const qint64 rxMs = QDateTime::currentMSecsSinceEpoch();
        QByteArray data;
        data.resize(int(m_socket->pendingDatagramSize()));
        m_socket->readDatagram(data.data(), data.size());

        PhoneFix fix;
        QString reason;
        if (!validateDatagram(data, m_lastFix.seq, rxMs, m_maxAccuracy, &fix, &reason)) {
            ++m_rejected;
            emit datagramRejected(reason);
            continue;
        }
        ++m_accepted;
        m_lastFix = fix;
        m_stale = false;
        m_staleTimer->start(m_staleTimeoutMs);
        emit fixReceived(fix);
    }
}

void PhoneLocationReceiver::onStaleCheck() {
    if (!m_stale) {
        m_stale = true;
        emit fixLost();
    }
}

bool PhoneLocationReceiver::validateDatagram(const QByteArray &data, qint64 lastAcceptedSeq,
                                             qint64 rxTimeMs, float maxAccuracyMeters,
                                             PhoneFix *fixOut, QString *reasonOut) {
    if (data.size() < 16 || data.size() > 1024) {
        if (reasonOut) *reasonOut = QStringLiteral("size");
        return false;
    }
    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        if (reasonOut) *reasonOut = QStringLiteral("json");
        return false;
    }
    const QJsonObject o = doc.object();

    if (o.value(QStringLiteral("version")).toInt(-1) != 1) {
        if (reasonOut) *reasonOut = QStringLiteral("version");
        return false;
    }
    const double lat = o.value(QStringLiteral("lat")).toDouble(0);
    const double lng = o.value(QStringLiteral("lng")).toDouble(0);
    if (lat < -90.0 || lat > 90.0 || lng < -180.0 || lng > 180.0
        || (lat == 0.0 && lng == 0.0)) {
        if (reasonOut) *reasonOut = QStringLiteral("range");
        return false;
    }
    const QJsonValue accValue = o.value(QStringLiteral("accuracy"));
    if (!accValue.isDouble()) {
        if (reasonOut) *reasonOut = QStringLiteral("accuracy-missing");
        return false;
    }
    const double accuracy = accValue.toDouble();
    if (accuracy <= 0.0 || accuracy > maxAccuracyMeters) {
        if (reasonOut) *reasonOut = QStringLiteral("accuracy");
        return false;
    }
    const qint64 seq = qint64(o.value(QStringLiteral("seq")).toDouble(-1));
    if (seq < 0 || seq <= lastAcceptedSeq) {
        if (reasonOut) *reasonOut = QStringLiteral("seq");
        return false;
    }

    if (fixOut) {
        fixOut->lat = lat;
        fixOut->lng = lng;
        fixOut->accuracyM = float(accuracy);
        fixOut->speedMps = float(o.value(QStringLiteral("speed")).toDouble(0));
        fixOut->bearingDeg = float(o.value(QStringLiteral("bearing")).toDouble(0));
        fixOut->seq = seq;
        fixOut->phoneTimeMs = qint64(o.value(QStringLiteral("time")).toDouble(0));
        fixOut->rxTimeMs = rxTimeMs;
    }
    return true;
}
