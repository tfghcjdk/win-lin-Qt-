#ifndef HMI_CAMERA_V4L2_H
#define HMI_CAMERA_V4L2_H

#include <QAtomicInt>
#include <QImage>
#include <QThread>

class CameraV4l2 : public QThread {
    Q_OBJECT
public:
    explicit CameraV4l2(QObject *parent = 0)
        : QThread(parent),device(QStringLiteral("/dev/video0")),pending(0) {}

    QString device;
    void acknowledge() { pending.storeRelease(0); }
    void stop() { requestInterruption(); }

signals:
    void frameReady(const QImage &image);
    void message(const QString &text);

protected:
    void run() override;

private:
    QAtomicInt pending;
};

#endif
