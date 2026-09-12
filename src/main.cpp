#include <QApplication>
#include <QFile>
#include <QFontDatabase>
#include <QTimer>
#include "core/system_manager.h"
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
    SystemManager::initializeClock();
    MainWindow window;
    window.show();
    return app.exec();
}
