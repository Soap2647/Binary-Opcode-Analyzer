#include "ThemeManager.h"
#include <QFile>
#include <QSettings>

ThemeManager& ThemeManager::instance() {
    static ThemeManager inst;
    return inst;
}

void ThemeManager::apply(Theme theme, QApplication& app) {
    m_current = theme;
    QString path = (theme == Theme::Dark) ? ":/themes/themes/dark.qss"
                                           : ":/themes/themes/light.qss";
    QFile f(path);
    if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        app.setStyleSheet(QString::fromUtf8(f.readAll()));
        f.close();
    }

    QSettings settings("OpcodeAnalyzer", "OpcodeAnalyzer");
    settings.setValue("theme", (theme == Theme::Dark) ? "dark" : "light");
}

void ThemeManager::toggle(QApplication& app) {
    apply((m_current == Theme::Dark) ? Theme::Light : Theme::Dark, app);
}
