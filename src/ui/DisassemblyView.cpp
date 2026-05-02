#include "DisassemblyView.h"
#include <QTableView>
#include <QVBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QColor>
#include <sstream>
#include <iomanip>

// ── DisassemblyModel ─────────────────────────────────────────────────────────

DisassemblyModel::DisassemblyModel(QObject* parent)
    : QAbstractTableModel(parent) {}

void DisassemblyModel::setProgram(const Program* prog) {
    beginResetModel();
    m_program = prog;
    endResetModel();
}

int DisassemblyModel::rowCount(const QModelIndex&) const {
    return m_program ? static_cast<int>(m_program->size()) : 0;
}

int DisassemblyModel::columnCount(const QModelIndex&) const {
    return NumCols;
}

QColor DisassemblyModel::categoryColor(const QString& cat) {
    if (cat == "Veri Transferi")  return QColor(0x3b, 0x82, 0xf6, 60);  // mavi
    if (cat == "Aritmetik")      return QColor(0x22, 0xc5, 0x5e, 60);  // yeşil
    if (cat == "Kontrol Akışı")  return QColor(0xef, 0x44, 0x44, 60);  // kırmızı
    if (cat == "Mantıksal")      return QColor(0xf5, 0x9e, 0x0b, 60);  // turuncu
    if (cat == "Sistem")         return QColor(0xa8, 0x55, 0xf7, 60);  // mor
    if (cat == "Bit Kaydırma")   return QColor(0x06, 0xb6, 0xd4, 60);  // cam göbeği
    if (cat == "String")         return QColor(0xec, 0x48, 0x99, 60);  // pembe
    if (cat == "FPU")            return QColor(0x84, 0xcc, 0x16, 60);  // yeşil-sarı
    if (cat == "Prefix")         return QColor(0x94, 0xa3, 0xb8, 60);  // gri
    return QColor(0x6b, 0x72, 0x80, 60);
}

QVariant DisassemblyModel::data(const QModelIndex& index, int role) const {
    if (!m_program || !index.isValid()) return {};
    const auto* instr = m_program->at(static_cast<size_t>(index.row()));
    if (!instr) return {};

    if (role == Qt::DisplayRole) {
        std::ostringstream ss;
        switch (index.column()) {
            case Address:
                ss << "0x" << std::hex << std::setw(8) << std::setfill('0')
                   << index.row();
                return QString::fromStdString(ss.str());
            case Hex:
                ss << std::hex << std::uppercase << std::setfill('0')
                   << std::setw(2) << (int)instr->opcode();
                return QString::fromStdString(ss.str());
            case Mnemonic:    return QString::fromStdString(instr->name());
            case Operands:    return QString::fromStdString(instr->operands());
            case Category:    return QString::fromStdString(instr->category());
            case Description: return QString::fromStdString(instr->description());
        }
    }

    if (role == Qt::ForegroundRole && index.column() == Category) {
        QString cat = QString::fromStdString(instr->category());
        if (cat == "Veri Transferi")  return QColor("#3b82f6");
        if (cat == "Aritmetik")       return QColor("#22c55e");
        if (cat == "Kontrol Akışı")   return QColor("#ef4444");
        if (cat == "Mantıksal")       return QColor("#f59e0b");
        if (cat == "Sistem")          return QColor("#a855f7");
        if (cat == "Bit Kaydırma")    return QColor("#06b6d4");
        if (cat == "String")          return QColor("#ec4899");
        if (cat == "FPU")             return QColor("#84cc16");
        if (cat == "Prefix")          return QColor("#94a3b8");
    }

    if (role == Qt::FontRole && index.column() == Hex) {
        QFont f;
        f.setFamily("Consolas, Courier New, monospace");
        return f;
    }

    return {};
}

QVariant DisassemblyModel::headerData(int section, Qt::Orientation orient, int role) const {
    if (orient != Qt::Horizontal || role != Qt::DisplayRole) return {};
    switch (section) {
        case Address:     return "Adres";
        case Hex:         return "Hex";
        case Mnemonic:    return "Mnemonic";
        case Operands:    return "Operandlar";
        case Category:    return "Kategori";
        case Description: return "Açıklama";
    }
    return {};
}

// ── DisassemblyView ──────────────────────────────────────────────────────────

DisassemblyView::DisassemblyView(QWidget* parent) : QWidget(parent) {
    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);

    m_info = new QLabel("  Binary bir dosya açın (.exe .dll .bin .elf vb.)", this);
    m_info->setObjectName("kpiLabel");
    m_info->setContentsMargins(12, 8, 12, 8);
    lay->addWidget(m_info);

    m_model = new DisassemblyModel(this);
    m_table = new QTableView(this);
    m_table->setModel(m_model);
    m_table->setAlternatingRowColors(true);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->verticalHeader()->setDefaultSectionSize(40); // Increased row height
    m_table->verticalHeader()->hide();
    m_table->setShowGrid(false);
    m_table->setFocusPolicy(Qt::NoFocus); // Modern feel
    m_table->setWordWrap(false);

    // Sütun genişlikleri
    m_table->setColumnWidth(DisassemblyModel::Address,  120);
    m_table->setColumnWidth(DisassemblyModel::Hex,      100);
    m_table->setColumnWidth(DisassemblyModel::Mnemonic, 120);
    m_table->setColumnWidth(DisassemblyModel::Operands, 120);
    m_table->setColumnWidth(DisassemblyModel::Category, 140);

    lay->addWidget(m_table);
}

void DisassemblyView::setProgram(const Program* program) {
    m_model->setProgram(program);
    if (program && !program->empty()) {
        m_info->setText(QString("  %1 byte analiz edildi").arg(program->size()));
    }
}

void DisassemblyView::clear() {
    m_model->setProgram(nullptr);
    m_info->setText("  Binary bir dosya açın (.exe .dll .bin .elf vb.)");
}
