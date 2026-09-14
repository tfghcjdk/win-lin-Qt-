#include "navigation_controller.h"
#include "coordinate_transform.h"
#include "vehicle_data_center.h"
#include <QDateTime>
#include <QStringList>
#include <math.h>

using CoordinateTransform::distanceMeters;

NavigationController::NavigationController(NavigationService *service,
                                           VehicleDataCenter *model,
                                           QObject *parent)
    : QObject(parent)
    , m_service(service)
    , m_model(model)
    , m_totalLengthMeters(0)
    , m_haveFix(false)
    , m_fixLat(0)
    , m_fixLng(0)
    , m_consecutiveOffRoute(0)
    , m_lastRequestMs(0)
    , m_arrived(false)
    , m_deviationMeters(60)
    , m_deviationSamples(5)
    , m_rerouteCooldownSec(30)
    , m_arrivalMeters(30)
{
    connect(m_service, &NavigationService::routeReady,
            this, &NavigationController::onRouteReady);
    connect(m_service, &NavigationService::routeFailed,
            this, &NavigationController::onRouteFailed);
}

void NavigationController::setDestination(const QString &gcjLngLat) {
    const QString dest = gcjLngLat.trimmed();
    if (dest == m_destination)
        return;
    m_destination = dest;
    if (m_route.isValid()) {
        emit logLine(QStringLiteral("destination changed to %1, rerouting").arg(dest));
        clearRoute();
        if (m_haveFix)
            requestRouteNow(QStringLiteral("destination-change"));
    }
}

void NavigationController::clearRoute() {
    m_route = NavigationRoute();
    m_points.clear();
    m_cumulative.clear();
    m_pointStep.clear();
    m_stepStart.clear();
    m_totalLengthMeters = 0;
    m_consecutiveOffRoute = 0;
    m_arrived = false;
    m_model->routeDistanceMeters = 0;
    m_model->routeDurationSeconds = 0;
    m_model->routeNextStepMeters = 0;
    m_model->routeNextInstruction.clear();
    m_model->routeNextRoad.clear();
    m_model->routePolyline.clear();
    m_model->routeArrived = false;
    m_model->notify();
}

bool NavigationController::mayRequestNow() const {
    return QDateTime::currentMSecsSinceEpoch() - m_lastRequestMs
           >= qint64(m_rerouteCooldownSec) * 1000;
}

void NavigationController::requestRouteNow(const QString &reason) {
    if (m_destination.isEmpty()) {
        emit logLine(QStringLiteral("reroute skipped (%1): no destination set").arg(reason));
        return;
    }
    if (!mayRequestNow()) {
        emit logLine(QStringLiteral("reroute skipped (%1): cooldown").arg(reason));
        return;
    }
    m_lastRequestMs = QDateTime::currentMSecsSinceEpoch();
    const QString origin = QStringLiteral("%1,%2")
        .arg(QString::number(m_fixLng, 'f', 6), QString::number(m_fixLat, 'f', 6));
    emit logLine(QStringLiteral("route request (%1) %2 -> %3").arg(reason, origin, m_destination));
    m_service->requestRoute(origin, m_destination);
}

void NavigationController::onPhoneFix(const PhoneFix &fix) {
    const CoordinateTransform::LatLng gcj =
        CoordinateTransform::wgs84ToGcj02(fix.lat, fix.lng);
    m_haveFix = true;
    m_fixLat = gcj.lat;
    m_fixLng = gcj.lng;

    m_model->hasPositionFix = true;
    m_model->vehicleLat = gcj.lat;
    m_model->vehicleLng = gcj.lng;
    m_model->vehicleAccuracyM = fix.accuracyM;

    if (m_arrived) {
        // Stay arrived until the fix leaves the arrival radius.
        const QStringList d = m_destination.split(QLatin1Char(','));
        if (d.size() == 2
            && distanceMeters(gcj.lat, gcj.lng, d.at(1).toDouble(), d.at(0).toDouble())
               > m_arrivalMeters * 2) {
            m_arrived = false;
            requestRouteNow(QStringLiteral("resume-after-arrival"));
        }
        m_model->notify();
        return;
    }

    if (!m_route.isValid()) {
        m_model->notify();
        requestRouteNow(m_haveFix ? QStringLiteral("first-fix") : QStringLiteral("init"));
        return;
    }

    // Arrival check against the destination coordinate.
    const QStringList d = m_destination.split(QLatin1Char(','));
    if (d.size() == 2
        && distanceMeters(gcj.lat, gcj.lng, d.at(1).toDouble(), d.at(0).toDouble())
           <= m_arrivalMeters) {
        m_arrived = true;
        m_model->routeArrived = true;
        emit logLine(QStringLiteral("arrived within %1 m of destination")
                     .arg(int(m_arrivalMeters)));
        emit arrived();
        m_model->notify();
        return;
    }

    const Projection proj = project(gcj.lat, gcj.lng);
    // stepIndex < 0 means the route has no usable geometry: skip the
    // off-route logic entirely instead of triggering phantom reroutes.
    if (proj.stepIndex >= 0 && proj.offRouteMeters > m_deviationMeters) {
        ++m_consecutiveOffRoute;
        if (m_consecutiveOffRoute >= m_deviationSamples) {
            m_consecutiveOffRoute = 0;
            requestRouteNow(QStringLiteral("off-route %1m").arg(int(proj.offRouteMeters)));
            m_model->notify();
            return;
        }
    } else {
        m_consecutiveOffRoute = 0;
    }

    publishProgress(proj, fix.speedMps);
    m_model->notify();
}

void NavigationController::onPhoneFixLost() {
    m_model->hasPositionFix = false;
    m_model->notify();
    emit logLine(QStringLiteral("phone fix lost (stale)"));
}

void NavigationController::onRouteReady(const NavigationRoute &route) {
    m_route = route;
    rebuildPolyline();
    m_arrived = false;
    m_consecutiveOffRoute = 0;
    m_model->routeArrived = false;

    QStringList merged;
    for (int i = 0; i < route.steps.size(); ++i)
        if (!route.steps.at(i).polyline.isEmpty())
            merged << route.steps.at(i).polyline;
    m_model->routePolyline = merged.join(QLatin1Char(';'));

    emit logLine(QStringLiteral("route updated distance=%1 duration=%2 steps=%3")
                 .arg(route.totalDistanceMeters).arg(route.totalDurationSeconds)
                 .arg(route.steps.size()));

    if (m_haveFix) {
        const Projection proj = project(m_fixLat, m_fixLng);
        publishProgress(proj, 0);
    } else {
        m_model->routeDistanceMeters = route.totalDistanceMeters;
        m_model->routeDurationSeconds = route.totalDurationSeconds;
        if (!route.steps.isEmpty()) {
            m_model->routeNextInstruction = route.steps.first().instruction;
            QString road = route.steps.first().roadName;
            for (int i = 1; road.isEmpty() && i < route.steps.size(); ++i)
                road = route.steps.at(i).roadName;
            m_model->routeNextRoad = road;
            m_model->routeNextStepMeters = route.steps.first().distanceMeters;
        }
    }
    m_model->notify();
}

void NavigationController::onRouteFailed(int errorKind, const QString &errorText) {
    emit logLine(QStringLiteral("route request failed kind=%1 msg=%2").arg(errorKind).arg(errorText));
}

void NavigationController::rebuildPolyline() {
    m_points.clear();
    m_cumulative.clear();
    m_pointStep.clear();
    m_stepStart.clear();
    m_totalLengthMeters = 0;

    double along = 0;
    for (int s = 0; s < m_route.steps.size(); ++s) {
        m_stepStart.append(along);
        const QStringList pairs = m_route.steps.at(s).polyline.split(
            QLatin1Char(';'), QString::SkipEmptyParts);
        for (int i = 0; i < pairs.size(); ++i) {
            const QStringList ll = pairs.at(i).split(QLatin1Char(','));
            if (ll.size() != 2)
                continue;
            const QPointF p(ll.at(0).toDouble(), ll.at(1).toDouble()); // x=lng y=lat
            if (!m_points.isEmpty())
                along += distanceMeters(m_points.last().y(), m_points.last().x(),
                                        p.y(), p.x());
            // Skip duplicate joint points between consecutive steps.
            if (!m_points.isEmpty()
                && distanceMeters(m_points.last().y(), m_points.last().x(), p.y(), p.x()) < 0.5)
                continue;
            m_points.append(p);
            m_cumulative.append(along);
            m_pointStep.append(s);
        }
    }
    m_totalLengthMeters = along;
}

NavigationController::Projection NavigationController::project(double lat, double lng) const {
    Projection best;
    if (m_points.size() < 2)
        return best;
    for (int i = 0; i + 1 < m_points.size(); ++i) {
        const QPointF a = m_points.at(i), b = m_points.at(i + 1);
        // Local equirectangular projection, meters.
        const double latRef = lat * 3.14159265358979 / 180.0;
        const double mx = 111320.0 * cos(latRef), my = 110540.0;
        const double ax = (a.x() - lng) * mx, ay = (a.y() - lat) * my;
        const double bx = (b.x() - lng) * mx, by = (b.y() - lat) * my;
        const double dx = bx - ax, dy = by - ay;
        const double len2 = dx * dx + dy * dy;
        double t = len2 > 0 ? -(ax * dx + ay * dy) / len2 : 0;
        if (t < 0) t = 0; else if (t > 1) t = 1;
        const double px = ax + t * dx, py = ay + t * dy;
        const double off = sqrt(px * px + py * py);
        if (off < best.offRouteMeters) {
            best.offRouteMeters = off;
            best.alongMeters = m_cumulative.at(i) + sqrt(len2) * t;
            best.stepIndex = m_pointStep.at(i);
        }
    }
    if (best.stepIndex >= 0 && best.stepIndex + 1 < m_stepStart.size())
        best.stepEndMeters = m_stepStart.at(best.stepIndex + 1);
    else
        best.stepEndMeters = m_totalLengthMeters;
    return best;
}

void NavigationController::publishProgress(const Projection &proj, double speedMps) {
    if (!m_route.isValid())
        return;
    if (proj.stepIndex < 0) {
        // No polyline geometry to project onto: still publish the route
        // totals and the first maneuver so the UI shows real data.
        m_model->routeDistanceMeters = m_route.totalDistanceMeters;
        m_model->routeDurationSeconds = m_route.totalDurationSeconds;
        const NavigationStep &step = m_route.steps.first();
        m_model->routeNextStepMeters = step.distanceMeters;
        m_model->routeNextInstruction = step.instruction;
        QString road = step.roadName;
        for (int i = 1; road.isEmpty() && i < m_route.steps.size(); ++i)
            road = m_route.steps.at(i).roadName;
        m_model->routeNextRoad = road;
        return;
    }
    const double remaining = qMax(0.0, m_totalLengthMeters - proj.alongMeters);
    // Never publish 0 while navigating: a 0 total flips the UI back to the
    // demo texts right before the arrival radius kicks in.
    m_model->routeDistanceMeters = qMax(1, int(remaining + 0.5));

    // Scale the API duration by remaining fraction; blend in live speed when moving.
    double seconds = remaining;
    if (m_route.totalDistanceMeters > 0)
        seconds = m_route.totalDurationSeconds * remaining / m_route.totalDistanceMeters;
    if (speedMps > 1.0)
        seconds = 0.5 * seconds + 0.5 * (remaining / speedMps);
    m_model->routeDurationSeconds = int(seconds + 0.5);

    const NavigationStep &step = m_route.steps.at(proj.stepIndex);
    m_model->routeNextStepMeters = int(qMax(0.0, proj.stepEndMeters - proj.alongMeters) + 0.5);
    m_model->routeNextInstruction = step.instruction;
    QString road = step.roadName;
    for (int i = proj.stepIndex + 1; road.isEmpty() && i < m_route.steps.size(); ++i)
        road = m_route.steps.at(i).roadName;
    m_model->routeNextRoad = road;
}
