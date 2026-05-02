#pragma once
#include <QWidget>
#include "analysis/Report.h"

class QChartView;

class ChartView : public QWidget {
    Q_OBJECT
public:
    explicit ChartView(QWidget* parent = nullptr);
    void setReport(const Report& report, bool darkMode);
    void clear();

private:
    QChartView* m_barView = nullptr;
    QChartView* m_pieView = nullptr;

    void buildBarChart(const Report& r, bool dark);
    void buildPieChart(const Report& r, bool dark);
};
