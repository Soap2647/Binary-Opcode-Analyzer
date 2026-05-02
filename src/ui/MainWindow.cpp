#include "MainWindow.h"
#include "SummaryView.h"
#include "DisassemblyView.h"
#include "ChartView.h"
#include "CompareView.h"
#include "ThemeManager.h"
#include "RecentFiles.h"
#include "core/Disassembler.h"
#include "analysis/EntropyAnalyzer.h"
#include "analysis/FrequencyAnalyzer.h"

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QSettings>
#include <QToolBar>
#include <QToolButton>
#include <QTabWidget>
#include <QSplitter>
#include <QListWidget>
#include <QLabel>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QSaveFile>
#include <QDesktopServices>
#include <QUrl>
#include <QStandardPaths>
#include <QFont>
#include <stdexcept>

MainWindow::MainWindow(QApplication* app, QWidget* parent)
    : QMainWindow(parent), m_app(app) {
    setWindowTitle("Binary Opcode Analiz Aracı");
    setMinimumSize(1100, 700);
    resize(1280, 800);

    setupMenuBar();
    setupToolBar();
    setupCentralWidget();
    setupStatusBar();

    // Kaydedilmiş temayı uygula
    QSettings settings("OpcodeAnalyzer", "OpcodeAnalyzer");
    QString savedTheme = settings.value("theme", "dark").toString();
    ThemeManager::instance().apply(
        savedTheme == "light" ? Theme::Light : Theme::Dark, *m_app);
    m_themeBtn->setText(ThemeManager::instance().themeIcon());

    updateRecentList();
}

void MainWindow::setupMenuBar() {
    auto* fileMenu = menuBar()->addMenu("Dosya");
    fileMenu->addAction("Aç…", this, &MainWindow::openFile, QKeySequence::Open);
    fileMenu->addSeparator();
    fileMenu->addAction("CSV Olarak Dışa Aktar…", this, &MainWindow::exportCSV);
    fileMenu->addAction("HTML Olarak Dışa Aktar…", this, &MainWindow::exportHTML);
    fileMenu->addSeparator();
    fileMenu->addAction("Çıkış", qApp, &QApplication::quit, QKeySequence::Quit);

    auto* viewMenu = menuBar()->addMenu("Görünüm");
    viewMenu->addAction("Temayı Değiştir", this, &MainWindow::toggleTheme, tr("Ctrl+T"));

    auto* helpMenu = menuBar()->addMenu("Yardım");
    helpMenu->addAction("Hakkında", this, [this] {
        QMessageBox::about(this, "Hakkında",
            "<b>Binary Opcode Analiz Aracı v2.0</b><br>"
            "Herhangi bir binary dosyayı (.exe, .dll, .sys, .bin, .elf)<br>"
            "x86 opcode düzeyinde analiz eder.<br><br>"
            "C++17 · Qt 6 · OOP · CMake");
    });
}

void MainWindow::setupToolBar() {
    auto* tb = addToolBar("Ana Araç Çubuğu");
    tb->setMovable(false);
    tb->setFloatable(false);

    auto* openBtn = new QToolButton(tb);
    openBtn->setText("📂  Binary Aç");
    connect(openBtn, &QToolButton::clicked, this, &MainWindow::openFile);
    tb->addWidget(openBtn);

    tb->addSeparator();

    auto* csvBtn = new QToolButton(tb);
    csvBtn->setText("⬇  CSV");
    connect(csvBtn, &QToolButton::clicked, this, &MainWindow::exportCSV);
    tb->addWidget(csvBtn);

    auto* htmlBtn = new QToolButton(tb);
    htmlBtn->setText("⬇  HTML");
    connect(htmlBtn, &QToolButton::clicked, this, &MainWindow::exportHTML);
    tb->addWidget(htmlBtn);

    // Sağa dayalı tema butonu
    auto* spacer = new QWidget(tb);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    tb->addWidget(spacer);

    m_themeBtn = new QToolButton(tb);
    m_themeBtn->setText("🌙");
    m_themeBtn->setToolTip("Temayı Değiştir (Ctrl+T)");
    connect(m_themeBtn, &QToolButton::clicked, this, &MainWindow::toggleTheme);
    tb->addWidget(m_themeBtn);
}

void MainWindow::setupCentralWidget() {
    auto* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setHandleWidth(1);
    setCentralWidget(splitter);

    // Sol panel: son açılan dosyalar
    auto* leftPanel = new QWidget(splitter);
    auto* leftLay   = new QVBoxLayout(leftPanel);
    leftLay->setContentsMargins(0, 0, 0, 0);
    leftLay->setSpacing(0);

    auto* recentTitle = new QLabel("  SON DOSYALAR", leftPanel);
    recentTitle->setObjectName("sectionTitle");
    recentTitle->setContentsMargins(20, 20, 10, 10);
    leftLay->addWidget(recentTitle);

    m_recentList = new QListWidget(leftPanel);
    m_recentList->setFixedWidth(260);
    connect(m_recentList, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item) {
        openFileFromPath(item->data(Qt::UserRole).toString());
    });
    leftLay->addWidget(m_recentList);
    splitter->addWidget(leftPanel);

    // Sağ panel: sekmeler
    m_tabs = new QTabWidget(splitter);
    m_tabs->setDocumentMode(true); // Modern look
    m_summary = new SummaryView(m_tabs);
    m_disasm  = new DisassemblyView(m_tabs);
    m_charts  = new ChartView(m_tabs);
    m_compare = new CompareView(m_tabs);

    m_tabs->addTab(m_summary, "📊  ÖZET");
    m_tabs->addTab(m_disasm,  "🔍  KOD ANALİZİ");
    m_tabs->addTab(m_charts,  "📈  GRAFİKLER");
    m_tabs->addTab(m_compare, "⚖  KIYASLAMA");

    connect(m_compare, &CompareView::requestLoadSecond, this, &MainWindow::loadSecondFile);
    splitter->addWidget(m_tabs);

    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
}

void MainWindow::setupStatusBar() {
    m_statusLabel = new QLabel("Hazır — .exe .dll .bin .elf vb. açabilirsiniz", this);
    statusBar()->addWidget(m_statusLabel);
}

void MainWindow::openFile() {
    QString path = QFileDialog::getOpenFileName(
        this, "Binary Dosya Aç", QString(),
        "Çalıştırılabilir Dosyalar (*.exe *.dll *.sys *.bin *.elf *.com *.ocx *.drv)"
        ";;Tüm Dosyalar (*)");
    if (!path.isEmpty())
        openFileFromPath(path);
}

void MainWindow::openFileFromPath(const QString& path) {
    try {
        Disassembler disasm;
        m_program = disasm.disassemble(path.toStdString());

        EntropyAnalyzer ea;
        m_currentReport = ea.analyze(m_program);

        applyReport(m_currentReport);
        RecentFiles::instance().add(path);
        updateRecentList();

        QFileInfo fi(path);
        double sizeKB = fi.size() / 1024.0;
        QString sizeStr = sizeKB >= 1024.0
            ? QString("%1 MB").arg(sizeKB / 1024.0, 0, 'f', 2)
            : QString("%1 KB").arg(sizeKB, 0, 'f', 1);
        updateStatus(QString("%1 yüklendi  ·  %2 byte analiz edildi  ·  %3  ·  Tema: %4")
                     .arg(fi.fileName())
                     .arg(m_program.size())
                     .arg(sizeStr)
                     .arg(ThemeManager::instance().themeName()));
        m_hasFile = true;

    } catch (const std::exception& ex) {
        QMessageBox::critical(this, "Hata", QString::fromStdString(ex.what()));
        updateStatus("Hata: " + QString::fromStdString(ex.what()));
    }
}

void MainWindow::applyReport(const Report& r) {
    m_summary->setReport(r);
    m_disasm->setProgram(&m_program);
    m_charts->setReport(r, ThemeManager::instance().isDark());
    m_compare->setFirstReport(r);
}

void MainWindow::loadSecondFile() {
    QString path = QFileDialog::getOpenFileName(
        this, "İkinci Binary Dosyayı Aç", QString(),
        "Çalıştırılabilir Dosyalar (*.exe *.dll *.sys *.bin *.elf *.com *.ocx *.drv)"
        ";;Tüm Dosyalar (*)");
    if (path.isEmpty()) return;

    try {
        Disassembler disasm;
        Program prog2 = disasm.disassemble(path.toStdString());
        EntropyAnalyzer ea;
        Report r2 = ea.analyze(prog2);
        m_compare->setSecondReport(r2);
        m_tabs->setCurrentIndex(3);
    } catch (const std::exception& ex) {
        QMessageBox::critical(this, "Hata", QString::fromStdString(ex.what()));
    }
}

void MainWindow::exportCSV() {
    if (m_currentReport.isEmpty()) {
        QMessageBox::information(this, "Bilgi", "Önce bir dosya açın.");
        return;
    }
    QString path = QFileDialog::getSaveFileName(
        this, "CSV Olarak Kaydet", "rapor.csv",
        "CSV Dosyaları (*.csv)");
    if (path.isEmpty()) return;

    QSaveFile f(path);
    if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        f.write(QString::fromStdString(m_currentReport.toCSV()).toUtf8());
        f.commit();
        updateStatus("CSV kaydedildi: " + path);
    }
}

void MainWindow::exportHTML() {
    if (m_currentReport.isEmpty()) {
        QMessageBox::information(this, "Bilgi", "Önce bir dosya açın.");
        return;
    }
    QString path = QFileDialog::getSaveFileName(
        this, "HTML Olarak Kaydet", "rapor.html",
        "HTML Dosyaları (*.html)");
    if (path.isEmpty()) return;

    QSaveFile f(path);
    if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        f.write(QString::fromStdString(m_currentReport.toHTML()).toUtf8());
        if (f.commit()) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(path));
            updateStatus("HTML kaydedildi: " + path);
        }
    }
}

void MainWindow::toggleTheme() {
    ThemeManager::instance().toggle(*m_app);
    m_themeBtn->setText(ThemeManager::instance().themeIcon());

    // Grafikleri temaya göre yenile
    if (m_hasFile && !m_currentReport.isEmpty())
        m_charts->setReport(m_currentReport, ThemeManager::instance().isDark());

    updateStatus(QString("Tema değiştirildi: %1").arg(ThemeManager::instance().themeName()));
}

void MainWindow::updateRecentList() {
    m_recentList->clear();
    for (const QString& path : RecentFiles::instance().files()) {
        QFileInfo fi(path);
        auto* item = new QListWidgetItem(fi.fileName(), m_recentList);
        item->setData(Qt::UserRole, path);
        item->setToolTip(path);
    }
}

void MainWindow::updateStatus(const QString& msg) {
    m_statusLabel->setText("  " + msg);
}
