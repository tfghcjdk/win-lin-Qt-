#ifndef PHONE_LOCATION_RECEIVER_H
#define PHONE_LOCATION_RECEIVER_H
#include <QObject>
#include <QString>

class QUdpSocket;
class QTimer;

// One validated position fix pushed by the phone app.
struct PhoneFix {
    double lat;        // WGS-84
    double lng;        // WGS-84
    float accuracyM;   // horizontal accuracy in meters
    float speedMps;    // m/s, may be 0
    float bearingDeg;  // course over ground, may be 0
    qint64 seq;        // monotonically increasing packet sequence
    qint64 phoneTimeMs;// phone-side epoch ms (informational)
    qint64 rxTimeMs;   // board receive time, epoch ms
    PhoneFix()
        : lat(0), lng(0), accuracyM(9999), speedMps(0), bearingDeg(0)
        , seq(-1), phoneTimeMs(0), rxTimeMs(0) {}
    bool valid() const { return seq >= 0; }
};

// Receives phone GPS fixes over UDP (default port 45454) and validates them.
//
// Expected datagram (UTF-8 JSON, one per second from the Android app):
//   {"version":1,"seq":42,"time":1757822400000,
//    "lat":39.989643,"lng":116.481028,
//    "accuracy":12.5,"speed":3.2,"bearing":91.0}
//
// A second datagram type sets the navigation destination from the phone app
// (user searches a Chinese place name on the phone; the app geocodes it via
// AMap and pushes the result). Coordinates are GCJ-02 already - the board
// must NOT run the WGS-84 conversion on them:
//   {"version":1,"type":"destination","name":"望京SOHO",
//    "lat":39.995800,"lng":116.480900}
//
// Validation rules (from the agreed protocol):
//   - version must be 1
//   - |lat| <= 90, |lng| <= 180, and not exactly (0,0)
//   - accuracy present and <= maxAccuracyMeters (default 50)
//   - seq must be greater than the last accepted seq (reorder/duplicate drop)
//   - a fix is stale if no valid packet arrives within staleTimeoutMs
//     (default 5000); fixLost() is emitted once per stale transition
//   - destination datagrams skip the seq/accuracy rules (user-triggered)
//
// Invalid datagrams are counted and reported via datagramRejected() for
// diagnostics, never thrown. All parsing is exception-free (QJsonDocument).
class PhoneLocationReceiver : public QObject {
    Q_OBJECT
public:
    explicit PhoneLocationReceiver(QObject *parent = 0);
    ~PhoneLocationReceiver();

    bool start(quint16 port = 45454);
    void stop();
    bool listening() const;

    void setMaxAccuracyMeters(float meters) { m_maxAccuracy = meters; }
    void setStaleTimeoutMs(int ms);

    PhoneFix lastFix() const { return m_lastFix; }
    quint64 acceptedCount() const { return m_accepted; }
    quint64 rejectedCount() const { return m_rejected; }

    // Pure datagram validator, exposed for unit tests. On success fills
    // fixOut (rxTimeMs stamped by caller-side clock) and returns true;
    // otherwise returns false and sets reasonOut (short stable token).
    static bool validateDatagram(const QByteArray &data, qint64 lastAcceptedSeq,
                                 qint64 rxTimeMs, float maxAccuracyMeters,
                                 PhoneFix *fixOut, QString *reasonOut);

    // Pure destination-datagram parser (type="destination", GCJ-02 coords).
    static bool parseDestinationDatagram(const QByteArray &data, QString *nameOut,
                                         double *latOut, double *lngOut,
                                         QString *reasonOut);

signals:
    void fixReceived(const PhoneFix &fix);
    void fixLost();                                   // went stale
    // Phone pushed a new destination (GCJ-02); name may be empty.
    void destinationReceived(const QString &name, double gcjLat, double gcjLng);
    void datagramRejected(const QString &reason);     // invalid packet
    void receiverError(const QString &message);       // bind failure etc.

private slots:
    void onReadyRead();
    void onStaleCheck();

private:
    QUdpSocket *m_socket;
    QTimer *m_staleTimer;
    PhoneFix m_lastFix;
    float m_maxAccuracy;
    int m_staleTimeoutMs;
    quint64 m_accepted;
    quint64 m_rejected;
    bool m_stale;          // currently in "no fresh fix" state
};
#endif
