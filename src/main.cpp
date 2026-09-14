#include <QApplication>
#include <QFile>
#include <QFontDatabase>
#include <QTimer>
#include <QDateTime>
#include <QTextStream>
#include "core/system_manager.h"
#include "core/navigation_service.h"
#include "ui/main_window.h"
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("NEV-SmartHMI"));
    app.setOrganizationName(QStringLiteral("NEV"));
    QFont font(QStringLiteral("Microsoft YaHei"));
    const QStringList families = QFontDatabase().families();
    if (!families.contains(font.family())) {
        if (families.contains(QStringLiteral("Noto Sans CJK SC"))) font.setFamily(QStringLiteral("Noto Sans CJK SC"));
        else font = app.font();
    }
    font.setPixelSize(13);
    app.setFont(font);
    QFile style(QStringLiteral(":/hmi/theme.qss"));
    if (style.open(QIODevice::ReadOnly)) app.setStyleSheet(QString::fromUtf8(style.readAll()));
    SystemManager::initializeWifi();
    SystemManager::initializeClock();
    MainWindow window;
    window.show();
#ifdef Q_OS_LINUX
    // Real AMap route on the board: async query with retry while WLAN settles.
    NavigationService *navigation = new NavigationService(QString(), &app);
    QTimer *navigationRetry = new QTimer(&app);
    navigationRetry->setSingleShot(true);
    int navigationAttempts = 0;
    const int navigationMaxAttempts = 5;
    // Append navigation events to a small log for board-side debugging.
    auto navLog = [](const QString &line) {
        QFile f(QStringLiteral("/Kd1234/config/navigation.log"));
        if (f.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream(&f) << QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"))
                            << QStringLiteral(" ") << line << QStringLiteral("\n");
        }
    };
    QObject::connect(navigation, &NavigationService::routeReady, &window, [&window, navLog](const NavigationRoute &route) {
        VehicleDataCenter *model = window.model();
        model->routeDistanceMeters = route.totalDistanceMeters;
        model->routeDurationSeconds = route.totalDurationSeconds;
        if (!route.steps.isEmpty()) {
            model->routeNextInstruction = route.steps.first().instruction;
            // First step often has no road name ("右转进入主路"); show the road
            // being entered, i.e. the first step that actually names one.
            QString road = route.steps.first().roadName;
            for (int i = 1; road.isEmpty() && i < route.steps.size(); ++i)
                road = route.steps.at(i).roadName;
            model->routeNextRoad = road;
            model->routeNextStepMeters = route.steps.first().distanceMeters;
        }
        model->notify();
        navLog(QStringLiteral("routeReady distance=%1 duration=%2 steps=%3 first='%4/%5'")
               .arg(route.totalDistanceMeters).arg(route.totalDurationSeconds).arg(route.steps.size())
               .arg(route.steps.isEmpty() ? QString() : route.steps.first().instruction,
                    route.steps.isEmpty() ? QString() : route.steps.first().roadName));
    });
    QObject::connect(navigation, &NavigationService::routeFailed, &app,
                     [&navigationAttempts, navigationMaxAttempts, navigationRetry, navLog](int kind, const QString &text) {
        navLog(QStringLiteral("routeFailed kind=%1 attempt=%2 msg=%3")
               .arg(kind).arg(navigationAttempts + 1).arg(text));
        if (++navigationAttempts < navigationMaxAttempts)
            navigationRetry->start(30000); // retry 30 s later (WLAN/DHCP may still be coming up)
    });
    QObject::connect(navigationRetry, &QTimer::timeout, navigation, &NavigationService::requestConfiguredRoute);
    navLog(QStringLiteral("startup requestConfiguredRoute"));
    navigation->requestConfiguredRoute();
#endif
    return app.exec();
}
