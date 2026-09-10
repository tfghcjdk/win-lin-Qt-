#ifndef HMI_CARDS_H
#define HMI_CARDS_H
#include <QWidget>
#include <QPainter>
#include <QPushButton>
#include <QSlider>
#include <QVector>
#include <QRectF>
#include "core/vehicle_data_center.h"

class HmiCard : public QWidget {
    Q_OBJECT
public:
    HmiCard(VehicleDataCenter *model, const QSize &design, QWidget *parent = 0);
protected:
    VehicleDataCenter *m;
    QSize base;
    void setup(QPainter &p, bool purple = false);
    QPushButton *button(const QString &id, const QString &label, const QRect &r, bool checkable = false);
    QSlider *slider(const QString &id, const QRect &r, int maximum);
    void place(QWidget *w, const QRect &r);
    void resizeEvent(QResizeEvent *event);
private:
    struct Placement { QWidget *widget; QRect rect; };
    QVector<Placement> placements;
};

class DashboardCard : public HmiCard {
public:
    enum Kind { Speed, Battery, Energy, Trip, Service, Slogan };
    DashboardCard(VehicleDataCenter *model, Kind kind, QWidget *parent = 0);
protected: void paintEvent(QPaintEvent *event);
private: Kind kind;
};
class ClimateCard : public HmiCard {
public:
    explicit ClimateCard(VehicleDataCenter *model, QWidget *parent = 0);
protected: void paintEvent(QPaintEvent *event);
private:
    QPushButton *minus, *plus, *autoButton, *acButton, *front, *rear, *recirc;
    QSlider *fanSlider;
    void refresh();
};
class NavigationCard : public HmiCard {
    Q_OBJECT
public:
    explicit NavigationCard(VehicleDataCenter *model, QWidget *parent = 0);
protected: void paintEvent(QPaintEvent *event);
signals: void openRequested();
};
class CameraCard : public HmiCard {
    Q_OBJECT
public:
    explicit CameraCard(VehicleDataCenter *model, QWidget *parent = 0);
protected: void paintEvent(QPaintEvent *event);
signals: void openRequested();
};
class MediaCard : public HmiCard {
public:
    explicit MediaCard(VehicleDataCenter *model, QWidget *parent = 0);
protected: void paintEvent(QPaintEvent *event);
private:
    QPushButton *play;
    QSlider *progress;
    void refresh();
};
#endif
