/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *layoutRoot;
    QWidget *widgetTopBar;
    QHBoxLayout *layoutTopBar;
    QLabel *labelDateTime;
    QSpacerItem *spacerTopBar;
    QLabel *labelConnection;
    QLabel *labelTopBattery;
    QWidget *widgetBody;
    QHBoxLayout *layoutBody;
    QFrame *frameSidebar;
    QVBoxLayout *layoutSidebar;
    QPushButton *btnHome;
    QPushButton *btnVehicle;
    QPushButton *btnNavigation;
    QPushButton *btnClimate;
    QPushButton *btnMedia;
    QPushButton *btnSettings;
    QSpacerItem *spacerSidebar;
    QStackedWidget *stackedWidgetPages;
    QWidget *pageHome;
    QVBoxLayout *layoutHome;
    QWidget *widgetHomeTop;
    QHBoxLayout *layoutHomeTop;
    QFrame *cardDriving;
    QVBoxLayout *layoutDriving;
    QLabel *labelDriveMode;
    QLabel *labelSpeed;
    QLabel *labelSpeedUnit;
    QLabel *labelGear;
    QFrame *cardVehicle;
    QVBoxLayout *layoutVehicle;
    QLabel *labelRange;
    QProgressBar *progressBattery;
    QLabel *labelCarImage;
    QLabel *labelTireFront;
    QLabel *labelTireRear;
    QFrame *cardEnergy;
    QVBoxLayout *layoutEnergy;
    QLabel *labelEnergyTitle;
    QLabel *labelMotorPower;
    QLabel *labelInstantConsumption;
    QLabel *labelOdometer;
    QWidget *widgetHomeMiddle;
    QHBoxLayout *layoutHomeMiddle;
    QFrame *cardClimate;
    QVBoxLayout *layoutClimate;
    QLabel *labelClimateTitle;
    QHBoxLayout *layoutTemperature;
    QPushButton *btnTempDown;
    QLabel *labelTemperature;
    QPushButton *btnTempUp;
    QSlider *sliderFanSpeed;
    QHBoxLayout *layoutClimateButtons;
    QPushButton *btnAC;
    QPushButton *btnAuto;
    QPushButton *btnRecirculation;
    QFrame *cardNavigation;
    QVBoxLayout *layoutNavigation;
    QLabel *labelNavigationTitle;
    QLabel *labelNavigationInstruction;
    QLabel *labelMapPreview;
    QLabel *labelRouteSummary;
    QFrame *cardCamera;
    QVBoxLayout *layoutCamera;
    QLabel *labelCameraTitle;
    QLabel *labelCameraPreview;
    QPushButton *btnOpenCamera;
    QFrame *cardMusic;
    QVBoxLayout *layoutMusic;
    QLabel *labelMusicTitle;
    QLabel *labelSongTitle;
    QLabel *labelArtist;
    QSlider *sliderMusicProgress;
    QHBoxLayout *layoutMusicButtons;
    QPushButton *btnPrevious;
    QPushButton *btnPlayPause;
    QPushButton *btnNext;
    QWidget *widgetHomeBottom;
    QHBoxLayout *layoutHomeBottom;
    QFrame *cardTrip;
    QVBoxLayout *layoutTrip;
    QLabel *labelTripTitle;
    QLabel *labelAverageConsumption;
    QLabel *labelTripSummary;
    QFrame *cardMaintenance;
    QVBoxLayout *layoutMaintenance;
    QLabel *labelMaintenanceTitle;
    QLabel *labelVehicleHealth;
    QLabel *labelServiceDistance;
    QWidget *pageVehicle;
    QVBoxLayout *layoutPageVehicle;
    QLabel *labelPageVehicle;
    QWidget *pageNavigation;
    QVBoxLayout *layoutPageNavigation;
    QLabel *labelPageNavigation;
    QWidget *pageClimate;
    QVBoxLayout *layoutPageClimate;
    QLabel *labelPageClimate;
    QWidget *pageMedia;
    QVBoxLayout *layoutPageMedia;
    QLabel *labelPageMedia;
    QWidget *pageSettings;
    QVBoxLayout *layoutPageSettings;
    QLabel *labelPageSettings;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1024, 600);
        MainWindow->setMinimumSize(QSize(1024, 600));
        MainWindow->setMaximumSize(QSize(1024, 600));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        layoutRoot = new QVBoxLayout(centralwidget);
        layoutRoot->setSpacing(8);
        layoutRoot->setObjectName(QString::fromUtf8("layoutRoot"));
        layoutRoot->setContentsMargins(8, 8, 8, 8);
        widgetTopBar = new QWidget(centralwidget);
        widgetTopBar->setObjectName(QString::fromUtf8("widgetTopBar"));
        widgetTopBar->setMinimumSize(QSize(0, 40));
        widgetTopBar->setMaximumSize(QSize(16777215, 40));
        layoutTopBar = new QHBoxLayout(widgetTopBar);
        layoutTopBar->setSpacing(16);
        layoutTopBar->setObjectName(QString::fromUtf8("layoutTopBar"));
        layoutTopBar->setContentsMargins(8, 8, 8, 8);
        labelDateTime = new QLabel(widgetTopBar);
        labelDateTime->setObjectName(QString::fromUtf8("labelDateTime"));

        layoutTopBar->addWidget(labelDateTime);

        spacerTopBar = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        layoutTopBar->addItem(spacerTopBar);

        labelConnection = new QLabel(widgetTopBar);
        labelConnection->setObjectName(QString::fromUtf8("labelConnection"));

        layoutTopBar->addWidget(labelConnection);

        labelTopBattery = new QLabel(widgetTopBar);
        labelTopBattery->setObjectName(QString::fromUtf8("labelTopBattery"));

        layoutTopBar->addWidget(labelTopBattery);


        layoutRoot->addWidget(widgetTopBar);

        widgetBody = new QWidget(centralwidget);
        widgetBody->setObjectName(QString::fromUtf8("widgetBody"));
        layoutBody = new QHBoxLayout(widgetBody);
        layoutBody->setSpacing(8);
        layoutBody->setObjectName(QString::fromUtf8("layoutBody"));
        layoutBody->setContentsMargins(0, 0, 0, 0);
        frameSidebar = new QFrame(widgetBody);
        frameSidebar->setObjectName(QString::fromUtf8("frameSidebar"));
        frameSidebar->setMinimumSize(QSize(88, 0));
        frameSidebar->setMaximumSize(QSize(88, 16777215));
        frameSidebar->setFrameShape(QFrame::NoFrame);
        layoutSidebar = new QVBoxLayout(frameSidebar);
        layoutSidebar->setSpacing(8);
        layoutSidebar->setObjectName(QString::fromUtf8("layoutSidebar"));
        layoutSidebar->setContentsMargins(4, 4, 4, 4);
        btnHome = new QPushButton(frameSidebar);
        btnHome->setObjectName(QString::fromUtf8("btnHome"));
        btnHome->setMinimumSize(QSize(0, 64));
        btnHome->setCheckable(true);
        btnHome->setChecked(true);
        btnHome->setAutoExclusive(true);

        layoutSidebar->addWidget(btnHome);

        btnVehicle = new QPushButton(frameSidebar);
        btnVehicle->setObjectName(QString::fromUtf8("btnVehicle"));
        btnVehicle->setMinimumSize(QSize(0, 64));
        btnVehicle->setCheckable(true);
        btnVehicle->setChecked(false);
        btnVehicle->setAutoExclusive(true);

        layoutSidebar->addWidget(btnVehicle);

        btnNavigation = new QPushButton(frameSidebar);
        btnNavigation->setObjectName(QString::fromUtf8("btnNavigation"));
        btnNavigation->setMinimumSize(QSize(0, 64));
        btnNavigation->setCheckable(true);
        btnNavigation->setChecked(false);
        btnNavigation->setAutoExclusive(true);

        layoutSidebar->addWidget(btnNavigation);

        btnClimate = new QPushButton(frameSidebar);
        btnClimate->setObjectName(QString::fromUtf8("btnClimate"));
        btnClimate->setMinimumSize(QSize(0, 64));
        btnClimate->setCheckable(true);
        btnClimate->setChecked(false);
        btnClimate->setAutoExclusive(true);

        layoutSidebar->addWidget(btnClimate);

        btnMedia = new QPushButton(frameSidebar);
        btnMedia->setObjectName(QString::fromUtf8("btnMedia"));
        btnMedia->setMinimumSize(QSize(0, 64));
        btnMedia->setCheckable(true);
        btnMedia->setChecked(false);
        btnMedia->setAutoExclusive(true);

        layoutSidebar->addWidget(btnMedia);

        btnSettings = new QPushButton(frameSidebar);
        btnSettings->setObjectName(QString::fromUtf8("btnSettings"));
        btnSettings->setMinimumSize(QSize(0, 64));
        btnSettings->setCheckable(true);
        btnSettings->setChecked(false);
        btnSettings->setAutoExclusive(true);

        layoutSidebar->addWidget(btnSettings);

        spacerSidebar = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

        layoutSidebar->addItem(spacerSidebar);


        layoutBody->addWidget(frameSidebar);

        stackedWidgetPages = new QStackedWidget(widgetBody);
        stackedWidgetPages->setObjectName(QString::fromUtf8("stackedWidgetPages"));
        pageHome = new QWidget();
        pageHome->setObjectName(QString::fromUtf8("pageHome"));
        layoutHome = new QVBoxLayout(pageHome);
        layoutHome->setSpacing(8);
        layoutHome->setObjectName(QString::fromUtf8("layoutHome"));
        layoutHome->setContentsMargins(0, 0, 0, 0);
        widgetHomeTop = new QWidget(pageHome);
        widgetHomeTop->setObjectName(QString::fromUtf8("widgetHomeTop"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widgetHomeTop->sizePolicy().hasHeightForWidth());
        widgetHomeTop->setSizePolicy(sizePolicy);
        layoutHomeTop = new QHBoxLayout(widgetHomeTop);
        layoutHomeTop->setSpacing(8);
        layoutHomeTop->setObjectName(QString::fromUtf8("layoutHomeTop"));
        layoutHomeTop->setContentsMargins(0, 0, 0, 0);
        cardDriving = new QFrame(widgetHomeTop);
        cardDriving->setObjectName(QString::fromUtf8("cardDriving"));
        cardDriving->setFrameShape(QFrame::NoFrame);
        sizePolicy.setHeightForWidth(cardDriving->sizePolicy().hasHeightForWidth());
        cardDriving->setSizePolicy(sizePolicy);
        layoutDriving = new QVBoxLayout(cardDriving);
        layoutDriving->setSpacing(6);
        layoutDriving->setObjectName(QString::fromUtf8("layoutDriving"));
        layoutDriving->setContentsMargins(10, 10, 10, 10);
        labelDriveMode = new QLabel(cardDriving);
        labelDriveMode->setObjectName(QString::fromUtf8("labelDriveMode"));

        layoutDriving->addWidget(labelDriveMode);

        labelSpeed = new QLabel(cardDriving);
        labelSpeed->setObjectName(QString::fromUtf8("labelSpeed"));
        labelSpeed->setAlignment(Qt::AlignCenter);

        layoutDriving->addWidget(labelSpeed);

        labelSpeedUnit = new QLabel(cardDriving);
        labelSpeedUnit->setObjectName(QString::fromUtf8("labelSpeedUnit"));
        labelSpeedUnit->setAlignment(Qt::AlignCenter);

        layoutDriving->addWidget(labelSpeedUnit);

        labelGear = new QLabel(cardDriving);
        labelGear->setObjectName(QString::fromUtf8("labelGear"));
        labelGear->setAlignment(Qt::AlignCenter);

        layoutDriving->addWidget(labelGear);


        layoutHomeTop->addWidget(cardDriving);

        cardVehicle = new QFrame(widgetHomeTop);
        cardVehicle->setObjectName(QString::fromUtf8("cardVehicle"));
        cardVehicle->setFrameShape(QFrame::NoFrame);
        sizePolicy.setHeightForWidth(cardVehicle->sizePolicy().hasHeightForWidth());
        cardVehicle->setSizePolicy(sizePolicy);
        layoutVehicle = new QVBoxLayout(cardVehicle);
        layoutVehicle->setSpacing(6);
        layoutVehicle->setObjectName(QString::fromUtf8("layoutVehicle"));
        layoutVehicle->setContentsMargins(10, 10, 10, 10);
        labelRange = new QLabel(cardVehicle);
        labelRange->setObjectName(QString::fromUtf8("labelRange"));

        layoutVehicle->addWidget(labelRange);

        progressBattery = new QProgressBar(cardVehicle);
        progressBattery->setObjectName(QString::fromUtf8("progressBattery"));
        progressBattery->setMinimum(0);
        progressBattery->setMaximum(100);
        progressBattery->setValue(80);

        layoutVehicle->addWidget(progressBattery);

        labelCarImage = new QLabel(cardVehicle);
        labelCarImage->setObjectName(QString::fromUtf8("labelCarImage"));
        labelCarImage->setAlignment(Qt::AlignCenter);
        sizePolicy.setHeightForWidth(labelCarImage->sizePolicy().hasHeightForWidth());
        labelCarImage->setSizePolicy(sizePolicy);
        labelCarImage->setMinimumSize(QSize(0, 60));
        labelCarImage->setWordWrap(true);

        layoutVehicle->addWidget(labelCarImage);

        labelTireFront = new QLabel(cardVehicle);
        labelTireFront->setObjectName(QString::fromUtf8("labelTireFront"));
        labelTireFront->setAlignment(Qt::AlignCenter);

        layoutVehicle->addWidget(labelTireFront);

        labelTireRear = new QLabel(cardVehicle);
        labelTireRear->setObjectName(QString::fromUtf8("labelTireRear"));
        labelTireRear->setAlignment(Qt::AlignCenter);

        layoutVehicle->addWidget(labelTireRear);


        layoutHomeTop->addWidget(cardVehicle);

        cardEnergy = new QFrame(widgetHomeTop);
        cardEnergy->setObjectName(QString::fromUtf8("cardEnergy"));
        cardEnergy->setFrameShape(QFrame::NoFrame);
        sizePolicy.setHeightForWidth(cardEnergy->sizePolicy().hasHeightForWidth());
        cardEnergy->setSizePolicy(sizePolicy);
        layoutEnergy = new QVBoxLayout(cardEnergy);
        layoutEnergy->setSpacing(6);
        layoutEnergy->setObjectName(QString::fromUtf8("layoutEnergy"));
        layoutEnergy->setContentsMargins(10, 10, 10, 10);
        labelEnergyTitle = new QLabel(cardEnergy);
        labelEnergyTitle->setObjectName(QString::fromUtf8("labelEnergyTitle"));

        layoutEnergy->addWidget(labelEnergyTitle);

        labelMotorPower = new QLabel(cardEnergy);
        labelMotorPower->setObjectName(QString::fromUtf8("labelMotorPower"));

        layoutEnergy->addWidget(labelMotorPower);

        labelInstantConsumption = new QLabel(cardEnergy);
        labelInstantConsumption->setObjectName(QString::fromUtf8("labelInstantConsumption"));

        layoutEnergy->addWidget(labelInstantConsumption);

        labelOdometer = new QLabel(cardEnergy);
        labelOdometer->setObjectName(QString::fromUtf8("labelOdometer"));

        layoutEnergy->addWidget(labelOdometer);


        layoutHomeTop->addWidget(cardEnergy);

        layoutHomeTop->setStretch(0, 3);
        layoutHomeTop->setStretch(1, 4);
        layoutHomeTop->setStretch(2, 2);

        layoutHome->addWidget(widgetHomeTop);

        widgetHomeMiddle = new QWidget(pageHome);
        widgetHomeMiddle->setObjectName(QString::fromUtf8("widgetHomeMiddle"));
        sizePolicy.setHeightForWidth(widgetHomeMiddle->sizePolicy().hasHeightForWidth());
        widgetHomeMiddle->setSizePolicy(sizePolicy);
        layoutHomeMiddle = new QHBoxLayout(widgetHomeMiddle);
        layoutHomeMiddle->setSpacing(8);
        layoutHomeMiddle->setObjectName(QString::fromUtf8("layoutHomeMiddle"));
        layoutHomeMiddle->setContentsMargins(0, 0, 0, 0);
        cardClimate = new QFrame(widgetHomeMiddle);
        cardClimate->setObjectName(QString::fromUtf8("cardClimate"));
        cardClimate->setFrameShape(QFrame::NoFrame);
        sizePolicy.setHeightForWidth(cardClimate->sizePolicy().hasHeightForWidth());
        cardClimate->setSizePolicy(sizePolicy);
        layoutClimate = new QVBoxLayout(cardClimate);
        layoutClimate->setSpacing(6);
        layoutClimate->setObjectName(QString::fromUtf8("layoutClimate"));
        layoutClimate->setContentsMargins(10, 10, 10, 10);
        labelClimateTitle = new QLabel(cardClimate);
        labelClimateTitle->setObjectName(QString::fromUtf8("labelClimateTitle"));

        layoutClimate->addWidget(labelClimateTitle);

        layoutTemperature = new QHBoxLayout();
        layoutTemperature->setSpacing(4);
        layoutTemperature->setObjectName(QString::fromUtf8("layoutTemperature"));
        layoutTemperature->setContentsMargins(0, 0, 0, 0);
        btnTempDown = new QPushButton(cardClimate);
        btnTempDown->setObjectName(QString::fromUtf8("btnTempDown"));

        layoutTemperature->addWidget(btnTempDown);

        labelTemperature = new QLabel(cardClimate);
        labelTemperature->setObjectName(QString::fromUtf8("labelTemperature"));
        labelTemperature->setAlignment(Qt::AlignCenter);

        layoutTemperature->addWidget(labelTemperature);

        btnTempUp = new QPushButton(cardClimate);
        btnTempUp->setObjectName(QString::fromUtf8("btnTempUp"));

        layoutTemperature->addWidget(btnTempUp);


        layoutClimate->addLayout(layoutTemperature);

        sliderFanSpeed = new QSlider(cardClimate);
        sliderFanSpeed->setObjectName(QString::fromUtf8("sliderFanSpeed"));
        sliderFanSpeed->setMinimum(32);
        sliderFanSpeed->setMaximum(60);
        sliderFanSpeed->setValue(48);
        sliderFanSpeed->setTracking(true);
        sliderFanSpeed->setOrientation(Qt::Horizontal);

        layoutClimate->addWidget(sliderFanSpeed);

        layoutClimateButtons = new QHBoxLayout();
        layoutClimateButtons->setSpacing(4);
        layoutClimateButtons->setObjectName(QString::fromUtf8("layoutClimateButtons"));
        layoutClimateButtons->setContentsMargins(0, 0, 0, 0);
        btnAC = new QPushButton(cardClimate);
        btnAC->setObjectName(QString::fromUtf8("btnAC"));
        btnAC->setCheckable(true);
        btnAC->setChecked(true);

        layoutClimateButtons->addWidget(btnAC);

        btnAuto = new QPushButton(cardClimate);
        btnAuto->setObjectName(QString::fromUtf8("btnAuto"));
        btnAuto->setCheckable(true);
        btnAuto->setChecked(true);

        layoutClimateButtons->addWidget(btnAuto);

        btnRecirculation = new QPushButton(cardClimate);
        btnRecirculation->setObjectName(QString::fromUtf8("btnRecirculation"));
        btnRecirculation->setCheckable(true);
        btnRecirculation->setChecked(false);

        layoutClimateButtons->addWidget(btnRecirculation);


        layoutClimate->addLayout(layoutClimateButtons);


        layoutHomeMiddle->addWidget(cardClimate);

        cardNavigation = new QFrame(widgetHomeMiddle);
        cardNavigation->setObjectName(QString::fromUtf8("cardNavigation"));
        cardNavigation->setFrameShape(QFrame::NoFrame);
        sizePolicy.setHeightForWidth(cardNavigation->sizePolicy().hasHeightForWidth());
        cardNavigation->setSizePolicy(sizePolicy);
        layoutNavigation = new QVBoxLayout(cardNavigation);
        layoutNavigation->setSpacing(6);
        layoutNavigation->setObjectName(QString::fromUtf8("layoutNavigation"));
        layoutNavigation->setContentsMargins(10, 10, 10, 10);
        labelNavigationTitle = new QLabel(cardNavigation);
        labelNavigationTitle->setObjectName(QString::fromUtf8("labelNavigationTitle"));

        layoutNavigation->addWidget(labelNavigationTitle);

        labelNavigationInstruction = new QLabel(cardNavigation);
        labelNavigationInstruction->setObjectName(QString::fromUtf8("labelNavigationInstruction"));

        layoutNavigation->addWidget(labelNavigationInstruction);

        labelMapPreview = new QLabel(cardNavigation);
        labelMapPreview->setObjectName(QString::fromUtf8("labelMapPreview"));
        labelMapPreview->setAlignment(Qt::AlignCenter);
        sizePolicy.setHeightForWidth(labelMapPreview->sizePolicy().hasHeightForWidth());
        labelMapPreview->setSizePolicy(sizePolicy);
        labelMapPreview->setMinimumSize(QSize(0, 0));
        labelMapPreview->setWordWrap(true);

        layoutNavigation->addWidget(labelMapPreview);

        labelRouteSummary = new QLabel(cardNavigation);
        labelRouteSummary->setObjectName(QString::fromUtf8("labelRouteSummary"));

        layoutNavigation->addWidget(labelRouteSummary);


        layoutHomeMiddle->addWidget(cardNavigation);

        cardCamera = new QFrame(widgetHomeMiddle);
        cardCamera->setObjectName(QString::fromUtf8("cardCamera"));
        cardCamera->setFrameShape(QFrame::NoFrame);
        sizePolicy.setHeightForWidth(cardCamera->sizePolicy().hasHeightForWidth());
        cardCamera->setSizePolicy(sizePolicy);
        layoutCamera = new QVBoxLayout(cardCamera);
        layoutCamera->setSpacing(6);
        layoutCamera->setObjectName(QString::fromUtf8("layoutCamera"));
        layoutCamera->setContentsMargins(10, 10, 10, 10);
        labelCameraTitle = new QLabel(cardCamera);
        labelCameraTitle->setObjectName(QString::fromUtf8("labelCameraTitle"));

        layoutCamera->addWidget(labelCameraTitle);

        labelCameraPreview = new QLabel(cardCamera);
        labelCameraPreview->setObjectName(QString::fromUtf8("labelCameraPreview"));
        labelCameraPreview->setAlignment(Qt::AlignCenter);
        sizePolicy.setHeightForWidth(labelCameraPreview->sizePolicy().hasHeightForWidth());
        labelCameraPreview->setSizePolicy(sizePolicy);
        labelCameraPreview->setMinimumSize(QSize(0, 0));
        labelCameraPreview->setWordWrap(true);

        layoutCamera->addWidget(labelCameraPreview);

        btnOpenCamera = new QPushButton(cardCamera);
        btnOpenCamera->setObjectName(QString::fromUtf8("btnOpenCamera"));

        layoutCamera->addWidget(btnOpenCamera);


        layoutHomeMiddle->addWidget(cardCamera);

        cardMusic = new QFrame(widgetHomeMiddle);
        cardMusic->setObjectName(QString::fromUtf8("cardMusic"));
        cardMusic->setFrameShape(QFrame::NoFrame);
        sizePolicy.setHeightForWidth(cardMusic->sizePolicy().hasHeightForWidth());
        cardMusic->setSizePolicy(sizePolicy);
        layoutMusic = new QVBoxLayout(cardMusic);
        layoutMusic->setSpacing(6);
        layoutMusic->setObjectName(QString::fromUtf8("layoutMusic"));
        layoutMusic->setContentsMargins(10, 10, 10, 10);
        labelMusicTitle = new QLabel(cardMusic);
        labelMusicTitle->setObjectName(QString::fromUtf8("labelMusicTitle"));

        layoutMusic->addWidget(labelMusicTitle);

        labelSongTitle = new QLabel(cardMusic);
        labelSongTitle->setObjectName(QString::fromUtf8("labelSongTitle"));
        labelSongTitle->setWordWrap(true);

        layoutMusic->addWidget(labelSongTitle);

        labelArtist = new QLabel(cardMusic);
        labelArtist->setObjectName(QString::fromUtf8("labelArtist"));

        layoutMusic->addWidget(labelArtist);

        sliderMusicProgress = new QSlider(cardMusic);
        sliderMusicProgress->setObjectName(QString::fromUtf8("sliderMusicProgress"));
        sliderMusicProgress->setMaximum(100);
        sliderMusicProgress->setValue(50);
        sliderMusicProgress->setOrientation(Qt::Horizontal);

        layoutMusic->addWidget(sliderMusicProgress);

        layoutMusicButtons = new QHBoxLayout();
        layoutMusicButtons->setSpacing(4);
        layoutMusicButtons->setObjectName(QString::fromUtf8("layoutMusicButtons"));
        layoutMusicButtons->setContentsMargins(0, 0, 0, 0);
        btnPrevious = new QPushButton(cardMusic);
        btnPrevious->setObjectName(QString::fromUtf8("btnPrevious"));

        layoutMusicButtons->addWidget(btnPrevious);

        btnPlayPause = new QPushButton(cardMusic);
        btnPlayPause->setObjectName(QString::fromUtf8("btnPlayPause"));

        layoutMusicButtons->addWidget(btnPlayPause);

        btnNext = new QPushButton(cardMusic);
        btnNext->setObjectName(QString::fromUtf8("btnNext"));

        layoutMusicButtons->addWidget(btnNext);


        layoutMusic->addLayout(layoutMusicButtons);


        layoutHomeMiddle->addWidget(cardMusic);

        layoutHomeMiddle->setStretch(0, 3);
        layoutHomeMiddle->setStretch(1, 3);
        layoutHomeMiddle->setStretch(2, 2);
        layoutHomeMiddle->setStretch(3, 2);

        layoutHome->addWidget(widgetHomeMiddle);

        widgetHomeBottom = new QWidget(pageHome);
        widgetHomeBottom->setObjectName(QString::fromUtf8("widgetHomeBottom"));
        sizePolicy.setHeightForWidth(widgetHomeBottom->sizePolicy().hasHeightForWidth());
        widgetHomeBottom->setSizePolicy(sizePolicy);
        layoutHomeBottom = new QHBoxLayout(widgetHomeBottom);
        layoutHomeBottom->setSpacing(8);
        layoutHomeBottom->setObjectName(QString::fromUtf8("layoutHomeBottom"));
        layoutHomeBottom->setContentsMargins(0, 0, 0, 0);
        cardTrip = new QFrame(widgetHomeBottom);
        cardTrip->setObjectName(QString::fromUtf8("cardTrip"));
        cardTrip->setFrameShape(QFrame::NoFrame);
        sizePolicy.setHeightForWidth(cardTrip->sizePolicy().hasHeightForWidth());
        cardTrip->setSizePolicy(sizePolicy);
        layoutTrip = new QVBoxLayout(cardTrip);
        layoutTrip->setSpacing(6);
        layoutTrip->setObjectName(QString::fromUtf8("layoutTrip"));
        layoutTrip->setContentsMargins(10, 10, 10, 10);
        labelTripTitle = new QLabel(cardTrip);
        labelTripTitle->setObjectName(QString::fromUtf8("labelTripTitle"));

        layoutTrip->addWidget(labelTripTitle);

        labelAverageConsumption = new QLabel(cardTrip);
        labelAverageConsumption->setObjectName(QString::fromUtf8("labelAverageConsumption"));

        layoutTrip->addWidget(labelAverageConsumption);

        labelTripSummary = new QLabel(cardTrip);
        labelTripSummary->setObjectName(QString::fromUtf8("labelTripSummary"));

        layoutTrip->addWidget(labelTripSummary);


        layoutHomeBottom->addWidget(cardTrip);

        cardMaintenance = new QFrame(widgetHomeBottom);
        cardMaintenance->setObjectName(QString::fromUtf8("cardMaintenance"));
        cardMaintenance->setFrameShape(QFrame::NoFrame);
        sizePolicy.setHeightForWidth(cardMaintenance->sizePolicy().hasHeightForWidth());
        cardMaintenance->setSizePolicy(sizePolicy);
        layoutMaintenance = new QVBoxLayout(cardMaintenance);
        layoutMaintenance->setSpacing(6);
        layoutMaintenance->setObjectName(QString::fromUtf8("layoutMaintenance"));
        layoutMaintenance->setContentsMargins(10, 10, 10, 10);
        labelMaintenanceTitle = new QLabel(cardMaintenance);
        labelMaintenanceTitle->setObjectName(QString::fromUtf8("labelMaintenanceTitle"));

        layoutMaintenance->addWidget(labelMaintenanceTitle);

        labelVehicleHealth = new QLabel(cardMaintenance);
        labelVehicleHealth->setObjectName(QString::fromUtf8("labelVehicleHealth"));

        layoutMaintenance->addWidget(labelVehicleHealth);

        labelServiceDistance = new QLabel(cardMaintenance);
        labelServiceDistance->setObjectName(QString::fromUtf8("labelServiceDistance"));

        layoutMaintenance->addWidget(labelServiceDistance);


        layoutHomeBottom->addWidget(cardMaintenance);

        layoutHomeBottom->setStretch(0, 1);
        layoutHomeBottom->setStretch(1, 1);

        layoutHome->addWidget(widgetHomeBottom);

        layoutHome->setStretch(0, 4);
        layoutHome->setStretch(1, 4);
        layoutHome->setStretch(2, 2);
        stackedWidgetPages->addWidget(pageHome);
        pageVehicle = new QWidget();
        pageVehicle->setObjectName(QString::fromUtf8("pageVehicle"));
        layoutPageVehicle = new QVBoxLayout(pageVehicle);
        layoutPageVehicle->setSpacing(8);
        layoutPageVehicle->setObjectName(QString::fromUtf8("layoutPageVehicle"));
        layoutPageVehicle->setContentsMargins(0, 0, 0, 0);
        labelPageVehicle = new QLabel(pageVehicle);
        labelPageVehicle->setObjectName(QString::fromUtf8("labelPageVehicle"));
        labelPageVehicle->setAlignment(Qt::AlignCenter);

        layoutPageVehicle->addWidget(labelPageVehicle);

        stackedWidgetPages->addWidget(pageVehicle);
        pageNavigation = new QWidget();
        pageNavigation->setObjectName(QString::fromUtf8("pageNavigation"));
        layoutPageNavigation = new QVBoxLayout(pageNavigation);
        layoutPageNavigation->setSpacing(8);
        layoutPageNavigation->setObjectName(QString::fromUtf8("layoutPageNavigation"));
        layoutPageNavigation->setContentsMargins(0, 0, 0, 0);
        labelPageNavigation = new QLabel(pageNavigation);
        labelPageNavigation->setObjectName(QString::fromUtf8("labelPageNavigation"));
        labelPageNavigation->setAlignment(Qt::AlignCenter);

        layoutPageNavigation->addWidget(labelPageNavigation);

        stackedWidgetPages->addWidget(pageNavigation);
        pageClimate = new QWidget();
        pageClimate->setObjectName(QString::fromUtf8("pageClimate"));
        layoutPageClimate = new QVBoxLayout(pageClimate);
        layoutPageClimate->setSpacing(8);
        layoutPageClimate->setObjectName(QString::fromUtf8("layoutPageClimate"));
        layoutPageClimate->setContentsMargins(0, 0, 0, 0);
        labelPageClimate = new QLabel(pageClimate);
        labelPageClimate->setObjectName(QString::fromUtf8("labelPageClimate"));
        labelPageClimate->setAlignment(Qt::AlignCenter);

        layoutPageClimate->addWidget(labelPageClimate);

        stackedWidgetPages->addWidget(pageClimate);
        pageMedia = new QWidget();
        pageMedia->setObjectName(QString::fromUtf8("pageMedia"));
        layoutPageMedia = new QVBoxLayout(pageMedia);
        layoutPageMedia->setSpacing(8);
        layoutPageMedia->setObjectName(QString::fromUtf8("layoutPageMedia"));
        layoutPageMedia->setContentsMargins(0, 0, 0, 0);
        labelPageMedia = new QLabel(pageMedia);
        labelPageMedia->setObjectName(QString::fromUtf8("labelPageMedia"));
        labelPageMedia->setAlignment(Qt::AlignCenter);

        layoutPageMedia->addWidget(labelPageMedia);

        stackedWidgetPages->addWidget(pageMedia);
        pageSettings = new QWidget();
        pageSettings->setObjectName(QString::fromUtf8("pageSettings"));
        layoutPageSettings = new QVBoxLayout(pageSettings);
        layoutPageSettings->setSpacing(8);
        layoutPageSettings->setObjectName(QString::fromUtf8("layoutPageSettings"));
        layoutPageSettings->setContentsMargins(0, 0, 0, 0);
        labelPageSettings = new QLabel(pageSettings);
        labelPageSettings->setObjectName(QString::fromUtf8("labelPageSettings"));
        labelPageSettings->setAlignment(Qt::AlignCenter);

        layoutPageSettings->addWidget(labelPageSettings);

        stackedWidgetPages->addWidget(pageSettings);

        layoutBody->addWidget(stackedWidgetPages);


        layoutRoot->addWidget(widgetBody);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        stackedWidgetPages->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "\346\226\260\350\203\275\346\272\220\350\275\246\350\275\275\347\263\273\347\273\237", nullptr));
        MainWindow->setStyleSheet(QCoreApplication::translate("MainWindow", "QWidget { color: #EAF4FF; font-family: \"Microsoft YaHei\"; font-size: 14px; }\n"
"QWidget#centralwidget { background-color: #061321; }\n"
"QWidget#widgetTopBar, QFrame#frameSidebar { background-color: #091D30; border-radius: 8px; }\n"
"QFrame#cardDriving, QFrame#cardVehicle, QFrame#cardEnergy, QFrame#cardClimate, QFrame#cardNavigation, QFrame#cardCamera, QFrame#cardMusic, QFrame#cardTrip, QFrame#cardMaintenance { background-color: #102840; border: 1px solid #193C58; border-radius: 10px; }\n"
"QLabel { background: transparent; border: none; }\n"
"QLabel#labelSpeed { font-size: 64px; font-weight: bold; color: #FFFFFF; }\n"
"QLabel#labelTemperature { font-size: 28px; font-weight: bold; }\n"
"QLabel#labelRange { font-size: 20px; font-weight: bold; }\n"
"QLabel#labelDriveMode { color: #18D8A0; }\n"
"QPushButton { background-color: #14344F; border: 1px solid #285577; border-radius: 8px; padding: 6px; min-height: 28px; }\n"
"QPushButton:hover { background-color: #1B4769; }\n"
"QPushButton:pressed { background-color:"
                        " #096BA3; }\n"
"QPushButton:checked { background-color: #087DB9; border-color: #24C8FF; color: #FFFFFF; }\n"
"QPushButton:disabled { background-color: #0C2235; border-color: #1A354B; color: #547086; }\n"
"QFrame#frameSidebar QPushButton { border: none; text-align: center; font-size: 16px; min-height: 64px; }\n"
"QProgressBar { background-color: #071B2C; border: 1px solid #285577; border-radius: 5px; text-align: center; min-height: 20px; }\n"
"QProgressBar::chunk { background-color: #12CDA0; border-radius: 4px; }\n"
"QSlider::groove:horizontal { height: 6px; background-color: #071B2C; border-radius: 3px; }\n"
"QSlider::sub-page:horizontal { background-color: #16B9EF; border-radius: 3px; }\n"
"QSlider::handle:horizontal { width: 16px; margin: -5px 0; background-color: #EAF4FF; border-radius: 8px; }", nullptr));
        labelDateTime->setText(QCoreApplication::translate("MainWindow", "2026\345\271\2649\346\234\2107\346\227\245  \346\230\237\346\234\237\344\270\200  14:36", nullptr));
        labelConnection->setText(QCoreApplication::translate("MainWindow", "4G   Wi-Fi   GPS", nullptr));
        labelTopBattery->setText(QCoreApplication::translate("MainWindow", "\347\224\265\351\207\217 80%", nullptr));
        btnHome->setText(QCoreApplication::translate("MainWindow", "\351\246\226\351\241\265", nullptr));
        btnVehicle->setText(QCoreApplication::translate("MainWindow", "\350\275\246\350\276\206\347\212\266\346\200\201", nullptr));
        btnNavigation->setText(QCoreApplication::translate("MainWindow", "\345\257\274\350\210\252", nullptr));
        btnClimate->setText(QCoreApplication::translate("MainWindow", "\347\251\272\350\260\203", nullptr));
        btnMedia->setText(QCoreApplication::translate("MainWindow", "\345\252\222\344\275\223", nullptr));
        btnSettings->setText(QCoreApplication::translate("MainWindow", "\350\256\276\347\275\256", nullptr));
        labelDriveMode->setText(QCoreApplication::translate("MainWindow", "READY   \347\273\217\346\265\216\346\250\241\345\274\217", nullptr));
        labelSpeed->setText(QCoreApplication::translate("MainWindow", "60", nullptr));
        labelSpeedUnit->setText(QCoreApplication::translate("MainWindow", "km/h", nullptr));
        labelGear->setText(QCoreApplication::translate("MainWindow", "P    R    N    [D]", nullptr));
        labelRange->setText(QCoreApplication::translate("MainWindow", "\345\211\251\344\275\231\347\273\255\350\210\252 428 km", nullptr));
        progressBattery->setFormat(QCoreApplication::translate("MainWindow", "\347\224\265\351\207\217 %p%", nullptr));
        labelCarImage->setText(QCoreApplication::translate("MainWindow", "\350\275\246\350\276\206\345\233\276\347\211\207\345\215\240\344\275\215", nullptr));
        labelCarImage->setStyleSheet(QCoreApplication::translate("MainWindow", "background-color: #091D30; color: #688BA7; border: 1px dashed #285577; border-radius: 6px;", nullptr));
        labelTireFront->setText(QCoreApplication::translate("MainWindow", "\345\267\246\345\211\215 2.5 bar    \345\217\263\345\211\215 2.5 bar", nullptr));
        labelTireRear->setText(QCoreApplication::translate("MainWindow", "\345\267\246\345\220\216 2.4 bar    \345\217\263\345\220\216 2.4 bar", nullptr));
        labelEnergyTitle->setText(QCoreApplication::translate("MainWindow", "\350\203\275\351\207\217\344\277\241\346\201\257", nullptr));
        labelEnergyTitle->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size: 16px; font-weight: bold; color: #8DDFFF;", nullptr));
        labelMotorPower->setText(QCoreApplication::translate("MainWindow", "\347\224\265\346\234\272\345\212\237\347\216\207 35 kW", nullptr));
        labelInstantConsumption->setText(QCoreApplication::translate("MainWindow", "\347\236\254\346\227\266\347\224\265\350\200\227\n"
"12.3 kWh/100km", nullptr));
        labelOdometer->setText(QCoreApplication::translate("MainWindow", "\347\264\257\350\256\241\351\207\214\347\250\213\n"
"5236 km", nullptr));
        labelClimateTitle->setText(QCoreApplication::translate("MainWindow", "\347\251\272\350\260\203\346\216\247\345\210\266", nullptr));
        labelClimateTitle->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size: 16px; font-weight: bold; color: #8DDFFF;", nullptr));
        btnTempDown->setText(QCoreApplication::translate("MainWindow", "\342\210\222", nullptr));
        labelTemperature->setText(QCoreApplication::translate("MainWindow", "24.0 \342\204\203", nullptr));
        btnTempUp->setText(QCoreApplication::translate("MainWindow", "+", nullptr));
#if QT_CONFIG(tooltip)
        sliderFanSpeed->setToolTip(QCoreApplication::translate("MainWindow", "\350\260\203\350\212\202\346\270\251\345\272\246\357\274\23216\357\275\23630\342\204\203", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(accessibility)
        sliderFanSpeed->setAccessibleName(QCoreApplication::translate("MainWindow", "\347\251\272\350\260\203\346\270\251\345\272\246", nullptr));
#endif // QT_CONFIG(accessibility)
        btnAC->setText(QCoreApplication::translate("MainWindow", "A/C", nullptr));
        btnAuto->setText(QCoreApplication::translate("MainWindow", "\350\207\252\345\212\250", nullptr));
        btnRecirculation->setText(QCoreApplication::translate("MainWindow", "\345\206\205\345\276\252\347\216\257", nullptr));
        labelNavigationTitle->setText(QCoreApplication::translate("MainWindow", "\345\257\274\350\210\252", nullptr));
        labelNavigationTitle->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size: 16px; font-weight: bold; color: #8DDFFF;", nullptr));
        labelNavigationInstruction->setText(QCoreApplication::translate("MainWindow", "\345\211\215\346\226\271 800 \347\261\263\345\217\263\350\275\254", nullptr));
        labelMapPreview->setText(QCoreApplication::translate("MainWindow", "\345\234\260\345\233\276\345\215\240\344\275\215", nullptr));
        labelMapPreview->setStyleSheet(QCoreApplication::translate("MainWindow", "background-color: #091D30; color: #688BA7; border: 1px dashed #285577; border-radius: 6px;", nullptr));
        labelRouteSummary->setText(QCoreApplication::translate("MainWindow", "12 \345\205\254\351\207\214 \302\267 28 \345\210\206\351\222\237", nullptr));
        labelCameraTitle->setText(QCoreApplication::translate("MainWindow", "\346\263\212\350\275\246\345\275\261\345\203\217", nullptr));
        labelCameraTitle->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size: 16px; font-weight: bold; color: #8DDFFF;", nullptr));
        labelCameraPreview->setText(QCoreApplication::translate("MainWindow", "\346\221\204\345\203\217\345\244\264\347\224\273\351\235\242\345\215\240\344\275\215", nullptr));
        labelCameraPreview->setStyleSheet(QCoreApplication::translate("MainWindow", "background-color: #091D30; color: #688BA7; border: 1px dashed #285577; border-radius: 6px;", nullptr));
        btnOpenCamera->setText(QCoreApplication::translate("MainWindow", "\346\237\245\347\234\213\345\275\261\345\203\217", nullptr));
        labelMusicTitle->setText(QCoreApplication::translate("MainWindow", "\351\237\263\344\271\220", nullptr));
        labelMusicTitle->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size: 16px; font-weight: bold; color: #8DDFFF;", nullptr));
        labelSongTitle->setText(QCoreApplication::translate("MainWindow", "\345\220\221\347\235\200\346\233\264\350\277\234\347\232\204\350\277\234\346\226\271", nullptr));
        labelArtist->setText(QCoreApplication::translate("MainWindow", "\350\277\275\345\205\211\350\200\205", nullptr));
        btnPrevious->setText(QCoreApplication::translate("MainWindow", "\342\200\271", nullptr));
#if QT_CONFIG(tooltip)
        btnPrevious->setToolTip(QCoreApplication::translate("MainWindow", "\344\270\212\344\270\200\351\246\226", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(accessibility)
        btnPrevious->setAccessibleName(QCoreApplication::translate("MainWindow", "\344\270\212\344\270\200\351\246\226", nullptr));
#endif // QT_CONFIG(accessibility)
        btnPlayPause->setText(QCoreApplication::translate("MainWindow", "\342\205\241", nullptr));
#if QT_CONFIG(tooltip)
        btnPlayPause->setToolTip(QCoreApplication::translate("MainWindow", "\346\232\202\345\201\234", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(accessibility)
        btnPlayPause->setAccessibleName(QCoreApplication::translate("MainWindow", "\346\232\202\345\201\234", nullptr));
#endif // QT_CONFIG(accessibility)
        btnNext->setText(QCoreApplication::translate("MainWindow", "\342\200\272", nullptr));
#if QT_CONFIG(tooltip)
        btnNext->setToolTip(QCoreApplication::translate("MainWindow", "\344\270\213\344\270\200\351\246\226", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(accessibility)
        btnNext->setAccessibleName(QCoreApplication::translate("MainWindow", "\344\270\213\344\270\200\351\246\226", nullptr));
#endif // QT_CONFIG(accessibility)
        labelTripTitle->setText(QCoreApplication::translate("MainWindow", "\350\241\214\350\275\246\344\277\241\346\201\257", nullptr));
        labelTripTitle->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size: 16px; font-weight: bold; color: #8DDFFF;", nullptr));
        labelAverageConsumption->setText(QCoreApplication::translate("MainWindow", "\345\271\263\345\235\207\347\224\265\350\200\227 13.2 kWh/100km", nullptr));
        labelTripSummary->setText(QCoreApplication::translate("MainWindow", "\346\234\254\346\254\241\351\207\214\347\250\213 125.6 km    \350\241\214\351\251\266\346\227\266\351\225\277 2 h 18 min", nullptr));
        labelMaintenanceTitle->setText(QCoreApplication::translate("MainWindow", "\346\217\220\351\206\222\344\270\216\347\273\264\346\212\244", nullptr));
        labelMaintenanceTitle->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size: 16px; font-weight: bold; color: #8DDFFF;", nullptr));
        labelVehicleHealth->setText(QCoreApplication::translate("MainWindow", "\345\275\223\345\211\215\346\227\240\346\225\205\351\232\234\357\274\214\350\275\246\350\276\206\347\212\266\346\200\201\350\211\257\345\245\275", nullptr));
        labelVehicleHealth->setStyleSheet(QCoreApplication::translate("MainWindow", "color: #18D8A0;", nullptr));
        labelServiceDistance->setText(QCoreApplication::translate("MainWindow", "\344\270\213\346\254\241\344\277\235\345\205\273\350\277\230\345\211\251 3280 km", nullptr));
        labelPageVehicle->setText(QCoreApplication::translate("MainWindow", "\350\275\246\350\276\206\347\212\266\346\200\201", nullptr));
        labelPageVehicle->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size: 28px; color: #8DDFFF;", nullptr));
        labelPageNavigation->setText(QCoreApplication::translate("MainWindow", "\345\257\274\350\210\252", nullptr));
        labelPageNavigation->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size: 28px; color: #8DDFFF;", nullptr));
        labelPageClimate->setText(QCoreApplication::translate("MainWindow", "\347\251\272\350\260\203", nullptr));
        labelPageClimate->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size: 28px; color: #8DDFFF;", nullptr));
        labelPageMedia->setText(QCoreApplication::translate("MainWindow", "\345\252\222\344\275\223", nullptr));
        labelPageMedia->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size: 28px; color: #8DDFFF;", nullptr));
        labelPageSettings->setText(QCoreApplication::translate("MainWindow", "\350\256\276\347\275\256", nullptr));
        labelPageSettings->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size: 28px; color: #8DDFFF;", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
