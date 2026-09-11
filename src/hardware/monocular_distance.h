#ifndef HMI_MONOCULAR_DISTANCE_H
#define HMI_MONOCULAR_DISTANCE_H

#include <QImage>
#include <QRect>
#include <QString>

class MonocularDistanceEstimator {
public:
    struct Measurement {
        Measurement() : updated(false), valid(false), distanceCm(-1) {}
        bool updated;
        bool valid;
        int distanceCm;
        QRect obstacleArea;
    };

    explicit MonocularDistanceEstimator(const QString &configPath=QStringLiteral("/etc/nev-smarthmi/camera_calibration.ini"));
    bool isReady() const { return ready; }
    QString errorString() const { return error; }
    static double groundDistanceCm(double pixelY,double focalY,double centerY,double heightCm,double pitchDegrees);
    Measurement measure(const QImage &image);

private:
    bool ready;
    QString error;
    int calibrationWidth,calibrationHeight,analysisMaxWidth,processEveryFrames,frameCounter,lastDistance,missCount;
    double fx,fy,cx,cy,k1,k2,p1,p2,k3,cameraHeightCm,pitchDegrees,roiTopRatio,minContourArea;
};

#endif
