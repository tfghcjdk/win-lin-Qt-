#include "vehicle_data_center.h"
#include <QtGlobal>
VehicleDataCenter::VehicleDataCenter(QObject *parent)
    : QObject(parent), temperature(24.0), fan(4), ac(true), automatic(true),
      frontDefrost(false), rearDefrost(false), recirculation(false), playing(true),
      routeActive(true), dataAvailable(true), lowPressure(false), position(137), track(0), volume(60) {}
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
void VehicleDataCenter::setFan(int value) { fan = qBound(0, value, 8); notify(); }
void VehicleDataCenter::setPosition(int value) { position = qBound(0, value, duration()); notify(); }
void VehicleDataCenter::setVolume(int value) { volume = qBound(0, value, 100); notify(); }
void VehicleDataCenter::changeTrack(int delta) { track = (track + delta + 3) % 3; position = 0; notify(); }
void VehicleDataCenter::setDataAvailable(bool available) { dataAvailable = available; notify(); }
void VehicleDataCenter::setLowPressure(bool low) { lowPressure = low; notify(); }
void VehicleDataCenter::tick() {
    if (playing) {
        if (position < duration()) ++position;
        else { track = (track + 1) % 3; position = 0; }
    }
    notify();
}
