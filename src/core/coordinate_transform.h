#ifndef COORDINATE_TRANSFORM_H
#define COORDINATE_TRANSFORM_H

// WGS-84 -> GCJ-02 ("Mars coordinates") conversion.
//
// The phone's LocationManager reports WGS-84; AMap expects GCJ-02.
// Pure functions, no Qt dependency, so the tests can exercise them
// with published reference vectors. Algorithm is the well-known public
// "eviltransform" formula used by every open-source GCJ-02 converter.
//
// Accuracy: ~1-2 m vs the official obfuscation, well below the 50 m
// fix-accuracy gate in PhoneLocationReceiver.
namespace CoordinateTransform {

struct LatLng {
    double lat;
    double lng;
    LatLng() : lat(0), lng(0) {}
    LatLng(double latitude, double longitude) : lat(latitude), lng(longitude) {}
};

// Rough China bounding box. Outside of it GCJ-02 == WGS-84 (no offset).
bool outOfChina(double lat, double lng);

// Convert one point. Input WGS-84, returns GCJ-02.
LatLng wgs84ToGcj02(double lat, double lng);

// Great-circle distance in meters (haversine, WGS-84 ellipsoid mean radius).
double distanceMeters(double lat1, double lng1, double lat2, double lng2);

} // namespace CoordinateTransform
#endif
