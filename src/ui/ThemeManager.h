#pragma once
#include <QString>
#include <QApplication>

enum class Theme { Light, Dark };

class ThemeManager {
public:
    static ThemeManager& instance();

    void apply(Theme theme, QApplication& app);
    void toggle(QApplication& app);

    Theme   current()  const { return m_current; }
    bool    isDark()   const { return m_current == Theme::Dark; }
    QString themeIcon() const { return isDark() ? "☀" : "🌙"; }
    QString themeName() const { return isDark() ? "Koyu" : "Açık"; }

private:
    ThemeManager() = default;
    Theme m_current = Theme::Dark;
};
