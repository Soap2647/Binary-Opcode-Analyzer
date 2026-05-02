#include "RecentFiles.h"
#include <QSettings>

RecentFiles& RecentFiles::instance() {
    static RecentFiles inst;
    return inst;
}

RecentFiles::RecentFiles() {
    load();
}

void RecentFiles::load() {
    QSettings s("OpcodeAnalyzer", "OpcodeAnalyzer");
    m_files = s.value("recentFiles").toStringList();
}

void RecentFiles::save() const {
    QSettings s("OpcodeAnalyzer", "OpcodeAnalyzer");
    s.setValue("recentFiles", m_files);
}

void RecentFiles::add(const QString& path) {
    m_files.removeAll(path);
    m_files.prepend(path);
    while (m_files.size() > MaxFiles)
        m_files.removeLast();
    save();
}

void RecentFiles::remove(const QString& path) {
    m_files.removeAll(path);
    save();
}

void RecentFiles::clear() {
    m_files.clear();
    save();
}
