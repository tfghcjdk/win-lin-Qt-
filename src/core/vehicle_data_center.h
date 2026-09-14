#ifndef VEHICLE_DATA_CENTER_H
#define VEHICLE_DATA_CENTER_H
#include <QObject>
#include <QString>

// UI demonstration state only. No CAN, audio output or vehicle commands are sent.
// Demo bounds are NOT production HVAC/TPMS calibration values.
class VehicleDataCenter : public QObject {
    Q_OBJECT
public:
    explicit VehicleDataCenter(QObject *parent = 0);
    double temperature;
    bool ac, automatic, frontDefrost, rearDefrost, recirculation;
    bool playing, routeActive, dataAvailable, lowPressure;
    int position, track, volume, rearObstacleDistanceCm;
    // Real AMap route (written by NavigationService; 0/empty keeps demo text).
    int routeDistanceMeters;
    int routeDurationSeconds;
    int routeNextStepMeters;
    QString routeNextInstruction;
    QString routeNextRoad;
    bool hasRealRoute() const { return routeDistanceMeters > 0; }
    // Live phone location (phone mode; GCJ-02 after conversion).
    bool hasPositionFix;
    bool routeArrived;                  // controller detected arrival
    double vehicleLat, vehicleLng;      // GCJ-02
    float vehicleAccuracyM;
    QString routePolyline;              // "lng,lat;lng,lat;..." merged GCJ-02
    QString routeDestinationName;       // Chinese name pushed by the phone app
    QString positionText() const;
    QString trackTitle() const;
    QString artist() const;
    int duration() const;
    void setTemperature(double value);
    void setPosition(int value);
    void setVolume(int value);
    void changeTrack(int delta);
    void setDataAvailable(bool available);
    void setLowPressure(bool low);
    void setRearObstacleDistance(int centimeters);
    bool rearObstacleWarning() const;
    void notify();
public slots:
    void tick();
signals:
    void changed();
};
#endif
