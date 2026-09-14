#include <QtTest>
#include <QApplication>
#include <QFile>
#include <QDir>
#include <QImage>
#include <QSlider>
#include <QDate>
#include <QTime>
#include "ui/main_window.h"
#include "ui/hmi_cards.h"
#include "core/navigation_service.h"
#include "core/coordinate_transform.h"
#include "core/phone_location_receiver.h"
#include "hardware/camera_v4l2.h"
#include "hardware/monocular_distance.h"
#include <cmath>

class UiTests : public QObject {
    Q_OBJECT
private:
    MainWindow *w;
    QPushButton *button(QWidget *scope, const char *name) {
        return scope->findChild<QPushButton *>(QString::fromLatin1(name));
    }
private slots:
    void initTestCase() {
        QFile f(":/hmi/theme.qss");QVERIFY(f.open(QIODevice::ReadOnly));qApp->setStyleSheet(QString::fromUtf8(f.readAll()));
        QFont font(QStringLiteral("Microsoft YaHei"));font.setPixelSize(13);qApp->setFont(font);
        w=new MainWindow;w->show();QTest::qWait(100);QCOMPARE(w->size(),QSize(1024,600));
        QVERIFY(!QImage(":/hmi/prototype.jpg").isNull());
    }
    void navigationAndBounds() {
        for(int i=0;i<6;++i){QPushButton *b=button(w,qPrintable(QString("nav%1").arg(i)));QVERIFY(b);QTest::mouseClick(b,Qt::LeftButton);QCOMPARE(w->currentPage(),i);QVERIFY(b->isChecked());}
        w->setPage(0);QWidget *home=w->findChild<QWidget *>("homePage");QVERIFY(home);
        const QList<HmiCard *> cards=home->findChildren<HmiCard *>(QString(),Qt::FindDirectChildrenOnly);QCOMPARE(cards.size(),10);
        for(int i=0;i<cards.size();++i){QVERIFY(home->rect().contains(cards[i]->geometry()));
            const QList<QWidget *> children=cards[i]->findChildren<QWidget *>(QString(),Qt::FindDirectChildrenOnly);
            for(int j=0;j<children.size();++j)QVERIFY2(cards[i]->rect().contains(children[j]->geometry()),qPrintable(children[j]->objectName()));
        }
    }
    void beijingClockUsesUtcPlusEight() {
        const QDateTime utc(QDate(2026,1,1),QTime(16,5),Qt::UTC);
        QCOMPARE(MainWindow::beijingClockText(utc),QStringLiteral("2026年1月2日   星期五   00:05:00"));
    }
    void cameraDefaultsToClearVideo9Capture() {
        CameraV4l2 camera;
        QCOMPARE(camera.device,QStringLiteral("/dev/video9"));
        QCOMPARE(camera.requestedSize,QSize(1280,720));
        QCOMPARE(camera.requestedFps,30);
        const double distance=MonocularDistanceEstimator::groundDistanceCm(360,700,360,60,30);
        QVERIFY(std::fabs(distance-103.923)<.01);
        QCOMPARE(MonocularDistanceEstimator::groundDistanceCm(360,0,360,60,30),-1.0);
    }
    void climateBoundariesAndSharedState() {
        QWidget *home=w->findChild<QWidget *>("homeClimate");QVERIFY(home);
        QPushButton *plus=button(home,"temperatureUp"),*minus=button(home,"temperatureDown");QVERIFY(plus&&minus);
        QTest::mouseClick(plus,Qt::LeftButton);QCOMPARE(w->model()->temperature,24.5);
        for(int i=0;i<40;++i)QTest::mouseClick(plus,Qt::LeftButton);QCOMPARE(w->model()->temperature,30.0);QVERIFY(!plus->isEnabled());
        for(int i=0;i<40;++i)QTest::mouseClick(minus,Qt::LeftButton);QCOMPARE(w->model()->temperature,16.0);QVERIFY(!minus->isEnabled());
        w->model()->setTemperature(24.0);QSlider *temperature=home->findChild<QSlider *>("temperatureSlider");QVERIFY(temperature);
        QCOMPARE(temperature->minimum(),32);QCOMPARE(temperature->maximum(),60);QCOMPARE(temperature->value(),48);
        temperature->setValue(49);QCOMPARE(w->model()->temperature,24.5);
        QTest::mouseClick(button(home,"climateAC"),Qt::LeftButton);QVERIFY(!w->model()->ac);
        QTest::mouseClick(button(home,"frontDefrost"),Qt::LeftButton);QVERIFY(w->model()->frontDefrost);
        QTest::mouseClick(button(home,"rearDefrost"),Qt::LeftButton);QVERIFY(w->model()->rearDefrost);
        QTest::mouseClick(button(home,"recirculation"),Qt::LeftButton);QVERIFY(w->model()->recirculation);
        w->setPage(3);QWidget *detail=w->findChild<QWidget *>("detailClimate");QVERIFY(detail);QVERIFY(button(detail,"frontDefrost")->isChecked());QVERIFY(!button(detail,"climateAC")->isChecked());
        QTest::mouseClick(button(detail,"temperatureUp"),Qt::LeftButton);QCOMPARE(w->model()->temperature,25.0);
        w->setPage(0);QCOMPARE(w->model()->temperature,25.0);
    }
    void mediaControls() {
        QWidget *media=w->findChild<QWidget *>("homeMedia");QVERIFY(media);VehicleDataCenter *m=w->model();
        m->playing=true;m->notify();QTest::mouseClick(button(media,"playPause"),Qt::LeftButton);QVERIFY(!m->playing);
        int position=m->position;m->tick();QCOMPARE(m->position,position);
        QTest::mouseClick(button(media,"playPause"),Qt::LeftButton);m->tick();QCOMPARE(m->position,position+1);
        QSlider *progress=media->findChild<QSlider *>("mediaProgress");QVERIFY(progress);progress->setValue(55);QCOMPARE(m->position,55);
        QTest::mouseClick(button(media,"nextTrack"),Qt::LeftButton);QCOMPARE(m->track,1);QCOMPARE(m->position,0);
        QTest::mouseClick(button(media,"previousTrack"),Qt::LeftButton);QCOMPARE(m->track,0);
        m->setPosition(m->duration());m->tick();QCOMPARE(m->track,1);QCOMPARE(m->position,0);
        w->setPage(4);QSlider *volume=w->findChild<QSlider *>("mediaVolume");QVERIFY(volume);volume->setValue(25);QCOMPARE(m->volume,25);
    }
    void navigationCameraAndScenarios() {
        w->setPage(2);QTest::mouseClick(button(w,"toggleRoute"),Qt::LeftButton);QVERIFY(!w->model()->routeActive);
        QTest::mouseClick(button(w,"toggleRoute"),Qt::LeftButton);QVERIFY(w->model()->routeActive);
        w->setPage(0);QTest::mouseClick(button(w,"openCamera"),Qt::LeftButton);QVERIFY(w->cameraIsOpen());
        QTest::keyClick(w,Qt::Key_Escape);QVERIFY(!w->cameraIsOpen());QCOMPARE(w->currentPage(),0);
        w->setPage(5);QTest::mouseClick(button(w,"toggleData"),Qt::LeftButton);QVERIFY(!w->model()->dataAvailable);
        QTest::mouseClick(button(w,"toggleData"),Qt::LeftButton);QVERIFY(w->model()->dataAvailable);
        QTest::mouseClick(button(w,"togglePressure"),Qt::LeftButton);QVERIFY(w->model()->lowPressure);
        QTest::mouseClick(button(w,"toggleObstacle"),Qt::LeftButton);QVERIFY(w->model()->rearObstacleWarning());QCOMPARE(w->model()->rearObstacleDistanceCm,55);
        w->model()->setRearObstacleDistance(81);QVERIFY(!w->model()->rearObstacleWarning());w->model()->setRearObstacleDistance(80);QVERIFY(w->model()->rearObstacleWarning());
        QTest::mouseClick(button(w,"toggleObstacle"),Qt::LeftButton);QVERIFY(!w->model()->rearObstacleWarning());
        QTest::keyClick(w,Qt::Key_Escape);QCOMPARE(w->currentPage(),0);
    }
    void screenshots() {
        VehicleDataCenter *m=w->model();m->temperature=24;m->ac=true;m->automatic=true;m->frontDefrost=false;m->rearDefrost=false;m->recirculation=false;m->track=0;m->position=137;m->playing=false;m->lowPressure=false;m->notify();
        QString out=!qgetenv("HMI_SCREENSHOT_DIR").isEmpty()?QString::fromLocal8Bit(qgetenv("HMI_SCREENSHOT_DIR")):"screenshots";
        QDir().mkpath(out);
        const QStringList names=QStringList()<<"home"<<"vehicle"<<"navigation"<<"climate"<<"media"<<"settings";
        for(int i=0;i<6;++i){w->setPage(i);QTest::qWait(20);QVERIFY(w->grab().save(out+"/"+names[i]+".png"));}
        w->setPage(0);m->setRearObstacleDistance(-1);w->showCamera();QTest::qWait(20);QVERIFY(w->grab().save(out+"/camera.png"));
        m->setRearObstacleDistance(55);QTest::qWait(20);QVERIFY(w->grab().save(out+"/camera-warning.png"));QTest::keyClick(w,Qt::Key_Escape);m->setRearObstacleDistance(-1);
        m->setDataAvailable(false);QTest::qWait(20);QVERIFY(w->grab().save(out+"/offline.png"));m->setDataAvailable(true);m->setLowPressure(true);QTest::qWait(20);QVERIFY(w->grab().save(out+"/low-pressure.png"));
    }
    void navigationRouteParser() {
        // Fixture first; fall back to the repo copy when running from another cwd.
        QString fixturePath=QStringLiteral("fixtures/route_v5_sample.json");
        if(!QFile::exists(fixturePath))fixturePath=QStringLiteral("../tests/fixtures/route_v5_sample.json");
        QFile fixture(fixturePath);QVERIFY2(fixture.open(QIODevice::ReadOnly),qPrintable(fixturePath));
        const QByteArray body=fixture.readAll();

        NavigationRoute route;QString error;
        QVERIFY2(NavigationService::parseRouteResponse(body,&route,&error),qPrintable(error));
        QCOMPARE(route.totalDistanceMeters,10430);
        QCOMPARE(route.totalDurationSeconds,1580);
        QCOMPARE(route.steps.size(),3);
        QCOMPARE(route.steps.first().roadName,QStringLiteral("望京街"));
        QCOMPARE(route.steps.first().distanceMeters,420);
        QVERIFY(route.steps.first().polyline.contains(QLatin1String("116.481028,39.989643")));
        QVERIFY(!route.steps.last().instruction.isEmpty());

        // API error payload must be classified, not parsed as a route.
        const QByteArray apiError=QByteArrayLiteral("{\"status\":\"0\",\"info\":\"INVALID_USER_KEY\",\"infocode\":\"20001\"}");
        QVERIFY(!NavigationService::parseRouteResponse(apiError,&route,&error));
        QVERIFY(error.contains(QLatin1String("20001")));

        // Valid status but no paths => no route.
        const QByteArray noRoute=QByteArrayLiteral("{\"status\":\"1\",\"info\":\"OK\",\"infocode\":\"10000\",\"route\":{\"paths\":[]}}");
        QVERIFY(!NavigationService::parseRouteResponse(noRoute,&route,&error));

        // Garbage input => parse error, no crash.
        QVERIFY(!NavigationService::parseRouteResponse(QByteArrayLiteral("not json"),&route,&error));

        // v5 has no per-step "polyline"; geometry comes from tmcs[].tmc_polyline.
        const QByteArray v5Tmcs=QByteArrayLiteral(
            "{\"status\":\"1\",\"info\":\"OK\",\"infocode\":\"10000\",\"route\":{\"paths\":[{"
            "\"distance\":\"500\",\"cost\":{\"duration\":\"120\"},\"steps\":[{"
            "\"instruction\":\"turn right onto main road\",\"step_distance\":\"43\",\"tmcs\":["
            "{\"tmc_polyline\":\"116.48,39.98;116.47,39.98\",\"tmc_status\":\"ok\",\"tmc_distance\":\"43\"}]},{"
            "\"instruction\":\"turn left\",\"road_name\":\"Furong St\",\"step_distance\":\"457\",\"tmcs\":["
            "{\"tmc_polyline\":\"116.47,39.98;116.46,39.97\",\"tmc_status\":\"ok\",\"tmc_distance\":\"457\"}]}"
            "]}]}}");
        QVERIFY2(NavigationService::parseRouteResponse(v5Tmcs,&route,&error),qPrintable(error));
        QCOMPARE(route.totalDurationSeconds,120);
        QVERIFY(route.steps.first().polyline.contains(QLatin1String("116.48,39.98")));
        QVERIFY(route.steps.first().polyline.contains(QLatin1String("116.47,39.98")));
        QVERIFY(route.steps.last().polyline.contains(QLatin1String("116.46,39.97")));
        QCOMPARE(route.steps.last().roadName,QStringLiteral("Furong St"));

        // Real data must flow into the data center and the demo card falls back cleanly.
        VehicleDataCenter *m=w->model();
        m->routeDistanceMeters=route.totalDistanceMeters;m->routeDurationSeconds=route.totalDurationSeconds;
        m->routeNextRoad=route.steps.first().roadName;m->routeNextInstruction=route.steps.first().instruction;
        m->routeNextStepMeters=route.steps.first().distanceMeters;
        QVERIFY(m->hasRealRoute());QCOMPARE(m->routeNextStepMeters,420);m->notify();
        m->routeDistanceMeters=0;m->routeDurationSeconds=0;m->routeNextStepMeters=0;m->routeNextRoad.clear();m->routeNextInstruction.clear();
        QVERIFY(!m->hasRealRoute());m->notify();
    }
    void coordinateTransformKnownVectors() {
        using CoordinateTransform::wgs84ToGcj02;
        using CoordinateTransform::outOfChina;
        // Published reference: Beijing (WGS-84) -> GCJ-02 offset ~+0.0060/+0.0065.
        const CoordinateTransform::LatLng gcj=wgs84ToGcj02(39.904200,116.407400);
        QVERIFY(gcj.lat>39.904200&&gcj.lat<39.912);
        QVERIFY(gcj.lng>116.407400&&gcj.lng<116.416);
        // Typical offsets are 300-700 m.
        const double offset=CoordinateTransform::distanceMeters(39.904200,116.407400,gcj.lat,gcj.lng);
        QVERIFY(offset>200&&offset<900);
        // Outside China the coordinate passes through unchanged.
        QVERIFY(outOfChina(35.0,140.0));
        const CoordinateTransform::LatLng same=wgs84ToGcj02(35.0,140.0);
        QCOMPARE(same.lat,35.0);QCOMPARE(same.lng,140.0);
        // Haversine sanity: 1 deg lat ~ 111 km.
        const double km=CoordinateTransform::distanceMeters(30,116,31,116);
        QVERIFY(km>108000&&km<114000);
    }
    void phoneFixValidation() {
        PhoneFix fix;QString reason;
        const QByteArray good=QByteArrayLiteral(
            "{\"version\":1,\"seq\":7,\"time\":1757822400000,\"lat\":39.989643,\"lng\":116.481028,"
            "\"accuracy\":12.5,\"speed\":3.2,\"bearing\":91.0}");
        QVERIFY2(PhoneLocationReceiver::validateDatagram(good,6,1000,50,&fix,&reason),qPrintable(reason));
        QCOMPARE(fix.seq,qint64(7));QCOMPARE(fix.lat,39.989643);QCOMPARE(fix.accuracyM,12.5f);
        // seq must strictly increase.
        QVERIFY(!PhoneLocationReceiver::validateDatagram(good,7,1001,50,&fix,&reason));QCOMPARE(reason,QStringLiteral("seq"));
        // accuracy gate.
        const QByteArray coarse=QByteArrayLiteral(
            "{\"version\":1,\"seq\":8,\"lat\":39.98,\"lng\":116.48,\"accuracy\":80}");
        QVERIFY(!PhoneLocationReceiver::validateDatagram(coarse,7,1002,50,&fix,&reason));QCOMPARE(reason,QStringLiteral("accuracy"));
        // missing accuracy field.
        const QByteArray noAcc=QByteArrayLiteral("{\"version\":1,\"seq\":8,\"lat\":39.98,\"lng\":116.48}");
        QVERIFY(!PhoneLocationReceiver::validateDatagram(noAcc,7,1003,50,&fix,&reason));
        // range checks.
        const QByteArray badLat=QByteArrayLiteral("{\"version\":1,\"seq\":8,\"lat\":95,\"lng\":116.48,\"accuracy\":5}");
        QVERIFY(!PhoneLocationReceiver::validateDatagram(badLat,7,1004,50,&fix,&reason));QCOMPARE(reason,QStringLiteral("range"));
        const QByteArray nullIsland=QByteArrayLiteral("{\"version\":1,\"seq\":8,\"lat\":0,\"lng\":0,\"accuracy\":5}");
        QVERIFY(!PhoneLocationReceiver::validateDatagram(nullIsland,7,1005,50,&fix,&reason));QCOMPARE(reason,QStringLiteral("range"));
        // version / garbage.
        QVERIFY(!PhoneLocationReceiver::validateDatagram(QByteArrayLiteral("{\"version\":2}"),7,1006,50,&fix,&reason));
        QVERIFY(!PhoneLocationReceiver::validateDatagram(QByteArrayLiteral("hello"),7,1007,50,&fix,&reason));
    }
    void destinationDatagramParsing() {
        // The phone app geocodes a Chinese place name and pushes GCJ-02.
        QString name, reason;double lat=0,lng=0;
        const QByteArray good=QString::fromUtf8(
            "{\"version\":1,\"type\":\"destination\",\"name\":\"望京SOHO\","
            "\"lat\":39.995800,\"lng\":116.480900}").toUtf8();
        QVERIFY2(PhoneLocationReceiver::parseDestinationDatagram(good,&name,&lat,&lng,&reason),qPrintable(reason));
        QCOMPARE(name,QStringLiteral("望京SOHO"));QCOMPARE(lat,39.995800);QCOMPARE(lng,116.480900);
        // name is optional.
        const QByteArray noName=QByteArrayLiteral(
            "{\"version\":1,\"type\":\"destination\",\"lat\":39.9,\"lng\":116.4}");
        QVERIFY(PhoneLocationReceiver::parseDestinationDatagram(noName,&name,&lat,&lng,&reason));
        QVERIFY(name.isEmpty());
        // fix datagrams are not destinations (fall through to fix validation).
        const QByteArray fixPkt=QByteArrayLiteral(
            "{\"version\":1,\"seq\":3,\"lat\":39.98,\"lng\":116.47,\"accuracy\":9}");
        QVERIFY(!PhoneLocationReceiver::parseDestinationDatagram(fixPkt,&name,&lat,&lng,&reason));
        QCOMPARE(reason,QStringLiteral("not-destination"));
        // explicit but malformed destination => rejected, never navigated to.
        const QByteArray badRange=QByteArrayLiteral(
            "{\"version\":1,\"type\":\"destination\",\"lat\":95,\"lng\":116.4}");
        QVERIFY(!PhoneLocationReceiver::parseDestinationDatagram(badRange,&name,&lat,&lng,&reason));
        QCOMPARE(reason,QStringLiteral("destination-invalid"));
        const QByteArray badVer=QByteArrayLiteral(
            "{\"version\":2,\"type\":\"destination\",\"lat\":39.9,\"lng\":116.4}");
        QVERIFY(!PhoneLocationReceiver::parseDestinationDatagram(badVer,&name,&lat,&lng,&reason));
        QCOMPARE(reason,QStringLiteral("destination-invalid"));
    }
    void cleanupTestCase(){delete w;}
};
QTEST_MAIN(UiTests)
#include "ui_tests.moc"
