#pragma once
#include <QWidget>
#include "analysis/Report.h"

class QLabel;
class QGridLayout;
class QFrame;

class SummaryView : public QWidget {
    Q_OBJECT
public:
    explicit SummaryView(QWidget* parent = nullptr);
    void setReport(const Report& report);
    void clear();

private:
    QLabel* m_totalVal    = nullptr;
    QLabel* m_uniqueVal   = nullptr;
    QLabel* m_entropyVal  = nullptr;
    QLabel* m_mostFreqVal = nullptr;

    QLabel* m_catLabel    = nullptr;

    static QFrame* makeCard(const QString& title, QLabel*& valueLabel);
};
