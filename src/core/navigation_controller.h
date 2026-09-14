#ifndef NAVIGATION_CONTROLLER_H
#define NAVIGATION_CONTROLLER_H
#include <QObject>
#include <QString>
#include <QVector>
#include <QPointF>
#include "navigation_service.h"
#include "phone_location_receiver.h"

class VehicleDataCenter;

// Turns raw phone fixes into a live, self-healing navigation session.
//
// Flow:
//   fixReceived (WGS-84) -> convert to GCJ-02 -> update model position
//   -> no route yet?            request route (origin=fix, dest=configured)
//   -> route active?            project fix onto the route polyline:
//        remaining distance/time, current step, meters to next maneuver
//        deviation > threshold for N consecutive samples -> re-request
//        (rate-limited by rerouteCooldownSeconds)
//        distance to destination <= arrivalRadiusMeters -> arrived
//
// The controller owns no sockets: it is fed by a PhoneLocationReceiver and
// drives a NavigationService, both supplied by main.cpp. Route data lives
// in VehicleDataCenter so all six UI pages observe it through notify().
//
// Threshold defaults follow the agreed plan: 60 m deviation x 5 samples,
// 30 s re-route cooldown, 30 m arrival radius.
class NavigationController : public QObject {
    Q_OBJECT
public:
    explicit NavigationController(NavigationService *service,
                                  VehicleDataCenter *model,
                                  QObject *parent = 0);

    void setDestination(const QString &gcjLngLat);   // "lng,lat" GCJ-02
    QString destination() const { return m_destination; }
    void clearRoute();                                // back to idle/demo

    void setDeviationThresholdMeters(double m) { m_deviationMeters = m; }
    void setDeviationSamples(int n) { m_deviationSamples = qMax(1, n); }
    void setRerouteCooldownSeconds(int s) { m_rerouteCooldownSec = qMax(5, s); }
    void setArrivalRadiusMeters(double m) { m_arrivalMeters = m; }

    bool navigating() const { return m_route.isValid(); }

public slots:
    void onPhoneFix(const PhoneFix &fix);
    void onPhoneFixLost();

signals:
    void arrived();
    void logLine(const QString &line);      // main.cpp appends to navigation.log

private slots:
    void onRouteReady(const NavigationRoute &route);
    void onRouteFailed(int errorKind, const QString &errorText);

private:
    struct Projection {
        double alongMeters;     // traveled distance along the polyline
        double offRouteMeters;  // perpendicular distance to nearest segment
        int stepIndex;          // step containing the nearest segment
        double stepEndMeters;   // along-distance to the end of that step
        Projection() : alongMeters(0), offRouteMeters(1e9), stepIndex(-1), stepEndMeters(0) {}
    };

    void requestRouteNow(const QString &reason);
    bool mayRequestNow() const;             // cooldown gate
    void rebuildPolyline();                 // merge step polylines
    Projection project(double lat, double lng) const;
    void publishProgress(const Projection &proj, double speedMps);

    NavigationService *m_service;
    VehicleDataCenter *m_model;
    QString m_destination;                  // "lng,lat" GCJ-02
    NavigationRoute m_route;
    QVector<QPointF> m_points;              // merged polyline, x=lng y=lat (GCJ-02)
    QVector<double> m_cumulative;           // meters from start, same size as m_points
    QVector<int> m_pointStep;               // step index per polyline point
    QVector<double> m_stepStart;            // along-meters where each step begins
    double m_totalLengthMeters;
    bool m_haveFix;
    double m_fixLat, m_fixLng;              // GCJ-02
    int m_consecutiveOffRoute;
    qint64 m_lastRequestMs;
    bool m_arrived;
    double m_deviationMeters;
    int m_deviationSamples;
    int m_rerouteCooldownSec;
    double m_arrivalMeters;
};
#endif
