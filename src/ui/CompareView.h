#pragma once
#include <QWidget>
#include <QAbstractTableModel>
#include "analysis/Report.h"

class QTableView;
class QLabel;
class QPushButton;

// ── Model ─────────────────────────────────────────────────────────────────────
class CompareModel : public QAbstractTableModel {
    Q_OBJECT
public:
    enum Column { Mnemonic, Count1, Count2, Difference, ChangePercent, NumCols };
    explicit CompareModel(QObject* parent = nullptr);
    void setDiffs(const std::vector<OpcodeDiff>& diffs);
    int     rowCount(const QModelIndex& = {})    const override;
    int     columnCount(const QModelIndex& = {}) const override;
    QVariant data(const QModelIndex&, int = Qt::DisplayRole) const override;
    QVariant headerData(int, Qt::Orientation, int = Qt::DisplayRole) const override;
private:
    std::vector<OpcodeDiff> m_diffs;
};

// ── View ──────────────────────────────────────────────────────────────────────
class CompareView : public QWidget {
    Q_OBJECT
public:
    explicit CompareView(QWidget* parent = nullptr);

    void setFirstReport(const Report& r);
    void setSecondReport(const Report& r);
    void runComparison();
    void clear();

signals:
    void requestLoadSecond();

private:
    Report       m_report1;
    Report       m_report2;
    bool         m_has1 = false;
    bool         m_has2 = false;

    QLabel*      m_file1Label  = nullptr;
    QLabel*      m_file2Label  = nullptr;
    QPushButton* m_loadBtn     = nullptr;
    QTableView*  m_table       = nullptr;
    CompareModel* m_model      = nullptr;
    QLabel*      m_statusLabel = nullptr;
};
