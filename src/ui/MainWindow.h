#pragma once
#include <QMainWindow>
#include <memory>
#include "core/Program.h"
#include "analysis/Report.h"

class QTabWidget;
class QListWidget;
class QSplitter;
class QLabel;
class QToolButton;
class SummaryView;
class DisassemblyView;
class ChartView;
class CompareView;
class QApplication;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QApplication* app, QWidget* parent = nullptr);

private slots:
    void openFile();
    void openFileFromPath(const QString& path);
    void exportCSV();
    void exportHTML();
    void toggleTheme();
    void loadSecondFile();

private:
    void setupMenuBar();
    void setupToolBar();
    void setupCentralWidget();
    void setupStatusBar();
    void applyReport(const Report& report);
    void updateRecentList();
    void updateStatus(const QString& msg);

    QApplication*    m_app         = nullptr;
    QTabWidget*      m_tabs        = nullptr;
    QListWidget*     m_recentList  = nullptr;
    SummaryView*     m_summary     = nullptr;
    DisassemblyView* m_disasm      = nullptr;
    ChartView*       m_charts      = nullptr;
    CompareView*     m_compare     = nullptr;
    QLabel*          m_statusLabel = nullptr;
    QToolButton*     m_themeBtn    = nullptr;

    Program m_program;
    Report  m_currentReport;
    bool    m_hasFile = false;
};
