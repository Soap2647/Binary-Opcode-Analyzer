#pragma once
#include <QWidget>
#include <QAbstractTableModel>
#include "core/Program.h"

class QTableView;
class QLabel;

// ── Model ────────────────────────────────────────────────────────────────────
class DisassemblyModel : public QAbstractTableModel {
    Q_OBJECT
public:
    enum Column { Address, Hex, Mnemonic, Operands, Category, Description, NumCols };

    explicit DisassemblyModel(QObject* parent = nullptr);
    void setProgram(const Program* prog);

    int     rowCount(const QModelIndex& = {})    const override;
    int     columnCount(const QModelIndex& = {}) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation, int role = Qt::DisplayRole) const override;

private:
    const Program* m_program = nullptr;

    static QColor categoryColor(const QString& cat);
};

// ── View ─────────────────────────────────────────────────────────────────────
class DisassemblyView : public QWidget {
    Q_OBJECT
public:
    explicit DisassemblyView(QWidget* parent = nullptr);
    void setProgram(const Program* program);
    void clear();

private:
    QTableView*      m_table = nullptr;
    DisassemblyModel* m_model = nullptr;
    QLabel*          m_info  = nullptr;
};
