#ifndef HMI_MAIN_WINDOW_H
#define HMI_MAIN_WINDOW_H
#include <QWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QDateTime>
#include "core/vehicle_data_center.h"
class MainWindow : public QWidget {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    VehicleDataCenter *model() const { return state; }
    int currentPage() const { return stack->currentIndex(); }
    void setPage(int index);
    bool cameraIsOpen() const;
    static QString beijingClockText(const QDateTime &utcNow);
public slots:
    void showCamera();
protected:
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);
private:
    VehicleDataCenter *state;
    QStackedWidget *stack;
    QWidget *cameraOverlay;
    QVector<QPushButton *> navButtons;
    QWidget *homePage();
    QWidget *vehiclePage();
    QWidget *navigationPage();
    QWidget *climatePage();
    QWidget *mediaPage();
    QWidget *settingsPage();
};
#endif
