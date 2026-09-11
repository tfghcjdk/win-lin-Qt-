#ifndef HMI_CAMERA_V4L2_H
#define HMI_CAMERA_V4L2_H

#include <QAtomicInt>
#include <QImage>
#include <QSize>
#include <QThread>

class CameraV4l2 : public QThread {
    Q_OBJECT
public:
    explicit CameraV4l2(QObject *parent = 0)
        : QThread(parent),device(QStringLiteral("/dev/video9")),requestedSize(1280,720),requestedFps(30),pending(0) {}

    QString device;
    QSize requestedSize;
    int requestedFps;
    void acknowledge() { pending.storeRelease(0); }
    void stop() { requestInterruption(); }

signals:
    void frameReady(const QImage &image);
    void obstacleDistanceReady(int centimeters);
    void message(const QString &text);

protected:
    void run() override;

private:
    QAtomicInt pending;
};

#endif
