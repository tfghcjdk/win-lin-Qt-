#include "mainwindow.h"
#include <QApplication>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include <QSlider>
#include <QDebug>
#include <QLayout>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    MainWindow window;
    window.setAttribute(Qt::WA_DontShowOnScreen);
    window.show();
    app.processEvents();
    int errors = 0;
    auto check = [&](bool ok, const char *message) {
        if (!ok) { qWarning() << message; ++errors; }
    };
    check(window.size() == QSize(1024, 600), "Window size is not 1024x600");
    auto pages = window.findChild<QStackedWidget *>("stackedWidgetPages");
    check(pages && pages->count() == 6 && pages->currentIndex() == 0, "Incorrect pages");
    for (auto widget : window.findChildren<QWidget *>()) {
        if (!widget->isVisible()) continue;
        if (widget->parentWidget() && !widget->parentWidget()->rect().contains(widget->geometry())) {
            qWarning() << "Outside parent:" << widget->objectName() << widget->geometry(); ++errors;
        }
        auto label = qobject_cast<QLabel *>(widget);
        if (label && !label->wordWrap() && label->width() < label->sizeHint().width()) {
            qWarning() << "Label too narrow:" << label->objectName() << label->size() << label->sizeHint(); ++errors;
        }
    }
    auto home = window.findChild<QPushButton *>("btnHome");
    auto vehicle = window.findChild<QPushButton *>("btnVehicle");
    check(home->isChecked() && !vehicle->isChecked(), "Wrong initial selection");
    check(home->height() >= 64 && vehicle->height() >= 64, "Sidebar buttons too short");
    vehicle->click();
    check(vehicle->isChecked() && !home->isChecked(), "Menu is not exclusive");
    home->click();
    auto temperature = window.findChild<QLabel *>("labelTemperature");
    auto up = window.findChild<QPushButton *>("btnTempUp");
    auto down = window.findChild<QPushButton *>("btnTempDown");
    auto temperatureIs = [&](const char *number) {
        return temperature->text() == QString::fromLatin1(number) + QStringLiteral(" \u2103");
    };
    check(temperatureIs("24.0"), "Wrong initial temperature");
    up->click(); check(temperatureIs("24.5"), "Temperature increase failed");
    down->click(); check(temperatureIs("24.0"), "Temperature decrease failed");
    for (int i = 0; i < 100; ++i) up->click();
    check(temperatureIs("30.0") && !up->isEnabled() && down->isEnabled(), "Upper limit failed");
    down->click(); check(temperatureIs("29.5") && up->isEnabled(), "Cannot leave upper limit");
    for (int i = 0; i < 100; ++i) down->click();
    check(temperatureIs("16.0") && !down->isEnabled() && up->isEnabled(), "Lower limit failed");
    up->click(); check(temperatureIs("16.5") && down->isEnabled(), "Cannot leave lower limit");
    for (int i = 0; i < 15; ++i) up->click();
    check(temperatureIs("24.0"), "Temperature did not restore");
    const char *climateNames[] = {"btnAC", "btnAuto", "btnRecirculation"};
    for (const char *name : climateNames) {
        auto button = window.findChild<QPushButton *>(name);
        const bool initial = button->isChecked();
        button->click(); check(button->isChecked() != initial, "Function did not toggle");
        button->click(); check(button->isChecked() == initial, "Function did not toggle back");
    }
    auto recirculation = window.findChild<QPushButton *>("btnRecirculation");
    recirculation->click();
    check(window.findChild<QPushButton *>("btnAC")->isChecked()
          && window.findChild<QPushButton *>("btnAuto")->isChecked()
          && recirculation->isChecked(), "Climate functions are not independent");
    recirculation->click();
    auto ac = window.findChild<QPushButton *>("btnAC");
    check(ac->isChecked(), "AC should start checked");
    ac->click(); check(!ac->isChecked(), "AC cannot toggle"); ac->click();
    auto fan = window.findChild<QSlider *>("sliderFanSpeed");
    check(fan->minimum() == 32 && fan->maximum() == 60 && fan->value() == 48,
          "Wrong temperature slider range or initial value");
    fan->setSliderDown(true);
    fan->setSliderPosition(51);
    check(temperatureIs("25.5") && fan->hasTracking(), "Temperature not updated while dragging");
    fan->setSliderDown(false);
    up->click(); check(temperatureIs("26.0") && fan->value() == 52, "Plus did not sync slider");
    down->click(); check(temperatureIs("25.5") && fan->value() == 51, "Minus did not sync slider");
    fan->setValue(32);
    check(temperatureIs("16.0") && !down->isEnabled() && up->isEnabled(), "Slider lower bound failed");
    fan->setValue(60);
    check(temperatureIs("30.0") && !up->isEnabled() && down->isEnabled(), "Slider upper bound failed");
    fan->setValue(48);
    check(temperatureIs("24.0") && up->isEnabled() && down->isEnabled(), "Slider restore failed");
    for (int i = 1; i < 6; ++i) {
        pages->setCurrentIndex(i); app.processEvents();
        check(pages->currentWidget()->isVisible(), "Page not visible");
    }
    pages->setCurrentIndex(0); app.processEvents();
    check(window.grab().save("homepage.png"), "Could not save preview");
    qInfo() << "UI verification errors:" << errors << "minimum hint:" << window.minimumSizeHint();
    return errors ? 1 : 0;
}
