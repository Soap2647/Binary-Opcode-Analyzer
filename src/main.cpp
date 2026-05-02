#include <QApplication>
#include <QSettings>
#include "ui/MainWindow.h"
#include "ui/ThemeManager.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("OpcodeAnalyzer");
    app.setOrganizationName("OpcodeAnalyzer");
    app.setApplicationVersion("1.0");

    // Kayıtlı temayı belirle ve uygula
    QSettings settings("OpcodeAnalyzer", "OpcodeAnalyzer");
    QString saved = settings.value("theme", "dark").toString();
    ThemeManager::instance().apply(
        saved == "light" ? Theme::Light : Theme::Dark, app);

    MainWindow window(&app);
    window.show();

    return app.exec();
}
