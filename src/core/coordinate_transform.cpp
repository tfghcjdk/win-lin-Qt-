#include "coordinate_transform.h"
#include <math.h>

namespace CoordinateTransform {

// Krasovsky 1940 ellipsoid parameters used by GCJ-02.
static const double kA = 6378245.0;
static const double kEe = 0.00669342162296594323; // eccentricity^2
static const double kPi = 3.1415926535897932384626;

bool outOfChina(double lat, double lng) {
    return lng < 72.004 || lng > 137.8347 || lat < 0.8293 || lat > 55.8271;
}

static double transformLat(double x, double y) {
    double ret = -100.0 + 2.0 * x + 3.0 * y + 0.2 * y * y + 0.1 * x * y
               + 0.2 * sqrt(fabs(x));
    ret += (20.0 * sin(6.0 * x * kPi) + 20.0 * sin(2.0 * x * kPi)) * 2.0 / 3.0;
    ret += (20.0 * sin(y * kPi) + 40.0 * sin(y / 3.0 * kPi)) * 2.0 / 3.0;
    ret += (160.0 * sin(y / 12.0 * kPi) + 320.0 * sin(y * kPi / 30.0)) * 2.0 / 3.0;
    return ret;
}

static double transformLng(double x, double y) {
    double ret = 300.0 + x + 2.0 * y + 0.1 * x * x + 0.1 * x * y
               + 0.1 * sqrt(fabs(x));
    ret += (20.0 * sin(6.0 * x * kPi) + 20.0 * sin(2.0 * x * kPi)) * 2.0 / 3.0;
    ret += (20.0 * sin(x * kPi) + 40.0 * sin(x / 3.0 * kPi)) * 2.0 / 3.0;
    ret += (150.0 * sin(x / 12.0 * kPi) + 300.0 * sin(x / 30.0 * kPi)) * 2.0 / 3.0;
    return ret;
}

LatLng wgs84ToGcj02(double lat, double lng) {
    if (outOfChina(lat, lng))
        return LatLng(lat, lng);
    double dLat = transformLat(lng - 105.0, lat - 35.0);
    double dLng = transformLng(lng - 105.0, lat - 35.0);
    const double radLat = lat / 180.0 * kPi;
    double magic = sin(radLat);
    magic = 1 - kEe * magic * magic;
    const double sqrtMagic = sqrt(magic);
    dLat = (dLat * 180.0) / ((kA * (1 - kEe)) / (magic * sqrtMagic) * kPi);
    dLng = (dLng * 180.0) / (kA / sqrtMagic * cos(radLat) * kPi);
    return LatLng(lat + dLat, lng + dLng);
}

double distanceMeters(double lat1, double lng1, double lat2, double lng2) {
    static const double kEarthRadius = 6371000.0;
    const double rad = kPi / 180.0;
    const double dLat = (lat2 - lat1) * rad;
    const double dLng = (lng2 - lng1) * rad;
    const double a = sin(dLat / 2) * sin(dLat / 2)
                   + cos(lat1 * rad) * cos(lat2 * rad) * sin(dLng / 2) * sin(dLng / 2);
    return 2.0 * kEarthRadius * atan2(sqrt(a), sqrt(1.0 - a));
}

} // namespace CoordinateTransform
