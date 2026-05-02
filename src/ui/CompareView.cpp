#include "CompareView.h"
#include "analysis/ComparisonAnalyzer.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableView>
#include <QHeaderView>
#include <QColor>
#include <sstream>
#include <iomanip>
#include <cmath>

// ── CompareModel ──────────────────────────────────────────────────────────────

CompareModel::CompareModel(QObject* parent) : QAbstractTableModel(parent) {}

void CompareModel::setDiffs(const std::vector<OpcodeDiff>& diffs) {
    beginResetModel();
    m_diffs = diffs;
    endResetModel();
}

int CompareModel::rowCount(const QModelIndex&) const {
    return static_cast<int>(m_diffs.size());
}
int CompareModel::columnCount(const QModelIndex&) const { return NumCols; }

QVariant CompareModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= (int)m_diffs.size()) return {};
    const auto& d = m_diffs[index.row()];

    if (role == Qt::DisplayRole) {
        std::ostringstream ss;
        switch (index.column()) {
            case Mnemonic:      return QString::fromStdString(d.mnemonic);
            case Count1:        return QString::number(d.count1);
            case Count2:        return QString::number(d.count2);
            case Difference:    return (d.difference >= 0 ? "+" : "") + QString::number(d.difference);
            case ChangePercent:
                ss << std::fixed << std::setprecision(1) << d.changePercent << "%";
                return QString::fromStdString(ss.str());
        }
    }

    if (role == Qt::ForegroundRole) {
        if (index.column() == Difference || index.column() == ChangePercent) {
            if (d.difference > 0) return QColor("#22c55e");
            if (d.difference < 0) return QColor("#ef4444");
        }
    }

    return {};
}

QVariant CompareModel::headerData(int section, Qt::Orientation orient, int role) const {
    if (orient != Qt::Horizontal || role != Qt::DisplayRole) return {};
    switch (section) {
        case Mnemonic:      return "Opcode";
        case Count1:        return "Dosya 1";
        case Count2:        return "Dosya 2";
        case Difference:    return "Fark";
        case ChangePercent: return "Değişim %";
    }
    return {};
}

// ── CompareView ───────────────────────────────────────────────────────────────

CompareView::CompareView(QWidget* parent) : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    root->setSpacing(12);
    root->setContentsMargins(20, 20, 20, 20);

    auto* title = new QLabel("Dosya Karşılaştırma", this);
    title->setObjectName("sectionTitle");
    root->addWidget(title);

    // Dosya satırı
    auto* fileRow = new QHBoxLayout;
    m_file1Label = new QLabel("Dosya 1: —", this);
    m_file2Label = new QLabel("Dosya 2: —", this);
    m_loadBtn    = new QPushButton("İkinci Dosyayı Yükle…", this);
    m_loadBtn->setObjectName("primaryButton");
    connect(m_loadBtn, &QPushButton::clicked, this, &CompareView::requestLoadSecond);

    fileRow->addWidget(m_file1Label);
    fileRow->addWidget(m_file2Label);
    fileRow->addStretch();
    fileRow->addWidget(m_loadBtn);
    root->addLayout(fileRow);

    m_statusLabel = new QLabel("Karşılaştırma için önce ana pencereden bir dosya, "
                               "ardından ikinci dosyayı yükleyin.", this);
    m_statusLabel->setObjectName("kpiLabel");
    root->addWidget(m_statusLabel);

    // Tablo
    m_model = new CompareModel(this);
    m_table = new QTableView(this);
    m_table->setModel(m_model);
    m_table->setAlternatingRowColors(true);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->verticalHeader()->hide();
    m_table->setShowGrid(false);
    m_table->setColumnWidth(CompareModel::Mnemonic, 90);
    m_table->setColumnWidth(CompareModel::Count1,   80);
    m_table->setColumnWidth(CompareModel::Count2,   80);
    m_table->setColumnWidth(CompareModel::Difference, 80);
    root->addWidget(m_table);
}

void CompareView::setFirstReport(const Report& r) {
    m_report1 = r;
    m_has1    = true;
    m_file1Label->setText("Dosya 1: " + QString::fromStdString(r.filename));
    if (m_has1 && m_has2) runComparison();
}

void CompareView::setSecondReport(const Report& r) {
    m_report2 = r;
    m_has2    = true;
    m_file2Label->setText("Dosya 2: " + QString::fromStdString(r.filename));
    if (m_has1 && m_has2) runComparison();
}

void CompareView::runComparison() {
    Report cmp = ComparisonAnalyzer::compare(m_report1, m_report2);
    m_model->setDiffs(cmp.diffs);
    m_statusLabel->setText(
        QString("Toplam farklı opcode: %1").arg(cmp.diffs.size()));
}

void CompareView::clear() {
    m_has1 = m_has2 = false;
    m_model->setDiffs({});
    m_file1Label->setText("Dosya 1: —");
    m_file2Label->setText("Dosya 2: —");
    m_statusLabel->setText("Karşılaştırma için önce ana pencereden bir dosya, "
                           "ardından ikinci dosyayı yükleyin.");
}
