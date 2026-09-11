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
        QCOMPARE(MainWindow::beijingClockText(utc),QStringLiteral("2026年1月2日   星期五   00:05"));
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
    void cleanupTestCase(){delete w;}
};
QTEST_MAIN(UiTests)
#include "ui_tests.moc"
