#include "vehicle_data_center.h"
#include <QtGlobal>
VehicleDataCenter::VehicleDataCenter(QObject *parent)
    : QObject(parent), temperature(24.0), ac(true), automatic(true),
      frontDefrost(false), rearDefrost(false), recirculation(false), playing(true),
      routeActive(true), dataAvailable(true), lowPressure(false), position(137), track(0), volume(60), rearObstacleDistanceCm(-1),
      routeDistanceMeters(0), routeDurationSeconds(0), routeNextStepMeters(0),
      hasPositionFix(false), routeArrived(false), vehicleLat(0), vehicleLng(0), vehicleAccuracyM(9999) {}
QString VehicleDataCenter::positionText() const {
    if (!hasPositionFix) return QStringLiteral("等待手机定位");
    return QStringLiteral("%1, %2 (±%3m)")
        .arg(QString::number(vehicleLng, 'f', 6),
             QString::number(vehicleLat, 'f', 6),
             QString::number(int(vehicleAccuracyM)));
}
QString VehicleDataCenter::trackTitle() const {
    const QString titles[] = {QStringLiteral("向着更远的远方"), QStringLiteral("沿途的风景"), QStringLiteral("城市之外")};
    return titles[track];
}
QString VehicleDataCenter::artist() const {
    const QString names[] = {QStringLiteral("追光者"), QStringLiteral("旅行日记"), QStringLiteral("夜航计划")};
    return names[track];
}
int VehicleDataCenter::duration() const { const int seconds[] = {276, 243, 208}; return seconds[track]; }
void VehicleDataCenter::notify() { emit changed(); }
void VehicleDataCenter::setTemperature(double value) { temperature = qBound(16.0, qRound(value * 2.0) / 2.0, 30.0); notify(); }
void VehicleDataCenter::setPosition(int value) { position = qBound(0, value, duration()); notify(); }
void VehicleDataCenter::setVolume(int value) { volume = qBound(0, value, 100); notify(); }
void VehicleDataCenter::changeTrack(int delta) { track = (track + delta + 3) % 3; position = 0; notify(); }
void VehicleDataCenter::setDataAvailable(bool available) { dataAvailable = available; notify(); }
void VehicleDataCenter::setLowPressure(bool low) { lowPressure = low; notify(); }
void VehicleDataCenter::setRearObstacleDistance(int centimeters) { rearObstacleDistanceCm = centimeters < 0 ? -1 : qBound(20, centimeters, 500); notify(); }
bool VehicleDataCenter::rearObstacleWarning() const { return rearObstacleDistanceCm >= 0 && rearObstacleDistanceCm <= 80; }
void VehicleDataCenter::tick() {
    if (playing) {
        if (position < duration()) ++position;
        else { track = (track + 1) % 3; position = 0; }
    }
    notify();
}
