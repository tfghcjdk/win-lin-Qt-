#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSignalBlocker>

namespace {
constexpr double MinimumTemperature = 16.0;
constexpr double MaximumTemperature = 30.0;
constexpr double TemperatureStep = 0.5;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->btnTempDown, &QPushButton::clicked, this, [this]() {
        setClimateTemperature(climateTemperature - TemperatureStep);
    });
    connect(ui->btnTempUp, &QPushButton::clicked, this, [this]() {
        setClimateTemperature(climateTemperature + TemperatureStep);
    });
    // Slider values are half-degree units: 32 = 16 C, 60 = 30 C.
    ui->sliderFanSpeed->setRange(qRound(MinimumTemperature / TemperatureStep),
                               qRound(MaximumTemperature / TemperatureStep));
    ui->sliderFanSpeed->setTracking(true);
    connect(ui->sliderFanSpeed, &QSlider::valueChanged, this, [this](int value) {
        setClimateTemperature(value * TemperatureStep);
    });
    setClimateTemperature(climateTemperature);

    // Each climate function is an independent, persistent on/off toggle.
    // The UI stylesheet uses :checked to display the active state.
    for (QPushButton *button : {ui->btnAC, ui->btnAuto, ui->btnRecirculation}) {
        button->setCheckable(true);
        button->setAutoExclusive(false);
    }
}

void MainWindow::setClimateTemperature(double temperature)
{
    climateTemperature = qBound(MinimumTemperature, temperature, MaximumTemperature);
    const QSignalBlocker sliderBlocker(ui->sliderFanSpeed);
    ui->sliderFanSpeed->setValue(qRound(climateTemperature / TemperatureStep));
    ui->labelTemperature->setText(
        QStringLiteral("%1 \u2103").arg(climateTemperature, 0, 'f', 1));
    ui->btnTempDown->setEnabled(climateTemperature > MinimumTemperature);
    ui->btnTempUp->setEnabled(climateTemperature < MaximumTemperature);
}

MainWindow::~MainWindow()
{
    delete ui;
}

