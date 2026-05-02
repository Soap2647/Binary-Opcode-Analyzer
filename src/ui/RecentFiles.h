#pragma once
#include <QStringList>

class RecentFiles {
public:
    static constexpr int MaxFiles = 10;

    static RecentFiles& instance();

    void        add(const QString& path);
    void        remove(const QString& path);
    void        clear();
    QStringList files() const { return m_files; }
    bool        isEmpty() const { return m_files.isEmpty(); }

private:
    RecentFiles();
    void load();
    void save() const;

    QStringList m_files;
};
