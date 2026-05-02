#include "ChartView.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <algorithm>
#include <vector>

ChartView::ChartView(QWidget* parent) : QWidget(parent) {
    auto* lay = new QHBoxLayout(this);
    lay->setSpacing(12);
    lay->setContentsMargins(12, 12, 12, 12);

    m_barView = new QChartView(this);
    m_barView->setRenderHint(QPainter::Antialiasing);

    m_pieView = new QChartView(this);
    m_pieView->setRenderHint(QPainter::Antialiasing);

    lay->addWidget(m_barView, 3);
    lay->addWidget(m_pieView, 2);

    clear();
}

void ChartView::setReport(const Report& r, bool dark) {
    buildBarChart(r, dark);
    buildPieChart(r, dark);
}

void ChartView::buildBarChart(const Report& r, bool dark) {
    auto* chart = new QChart;
    chart->setTheme(dark ? QChart::ChartThemeDark : QChart::ChartThemeLight);
    chart->setTitle("Opcode Frekans Dağılımı");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setVisible(false);

    // En çok kullanılan 15 opcode'u al
    std::vector<std::pair<std::string, uint32_t>> sorted;
    for (const auto& [mnem, stats] : r.opcodeFreq)
        sorted.emplace_back(mnem, stats.count);
    std::sort(sorted.begin(), sorted.end(),
              [](const auto& a, const auto& b){ return a.second > b.second; });
    if (sorted.size() > 15) sorted.resize(15);

    auto* barSet = new QBarSet("Sayı");
    QStringList cats;
    for (const auto& [mnem, cnt] : sorted) {
        *barSet << cnt;
        cats << QString::fromStdString(mnem);
    }
    barSet->setColor(dark ? QColor("#89b4fa") : QColor("#2563eb"));

    auto* series = new QBarSeries;
    series->append(barSet);
    chart->addSeries(series);

    auto* axisX = new QBarCategoryAxis;
    axisX->append(cats);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto* axisY = new QValueAxis;
    axisY->setTitleText("Sayı");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->setBackgroundVisible(false);
    m_barView->setChart(chart);
}

void ChartView::buildPieChart(const Report& r, bool dark) {
    auto* chart = new QChart;
    chart->setTheme(dark ? QChart::ChartThemeDark : QChart::ChartThemeLight);
    chart->setTitle("Kategori Dağılımı");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    auto* series = new QPieSeries;
    QList<QColor> colors = {
        QColor("#3b82f6"), QColor("#22c55e"), QColor("#ef4444"),
        QColor("#f59e0b"), QColor("#a855f7"), QColor("#6b7280")
    };
    int ci = 0;
    for (const auto& [cat, cnt] : r.categoryFreq) {
        auto* slice = series->append(QString::fromStdString(cat), cnt);
        slice->setColor(colors[ci++ % colors.size()]);
        slice->setLabelVisible(true);
        slice->setLabelColor(dark ? Qt::white : Qt::black);
    }
    series->setHoleSize(0.35);
    chart->addSeries(series);
    chart->setBackgroundVisible(false);
    chart->legend()->setAlignment(Qt::AlignRight);

    m_pieView->setChart(chart);
}

void ChartView::clear() {
    auto* empty = new QChart;
    empty->setTitle("Grafik için dosya açın");
    empty->setBackgroundVisible(false);
    m_barView->setChart(empty);

    auto* empty2 = new QChart;
    empty2->setTitle("Grafik için dosya açın");
    empty2->setBackgroundVisible(false);
    m_pieView->setChart(empty2);
}
