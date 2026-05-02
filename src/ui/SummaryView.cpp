#include "SummaryView.h"
#include <QLabel>
#include <QFrame>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <iomanip>
#include <sstream>

SummaryView::SummaryView(QWidget* parent) : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    root->setSpacing(30);
    root->setContentsMargins(40, 40, 40, 40);

    auto* title = new QLabel("Analiz Özeti", this);
    title->setObjectName("sectionTitle");
    root->addWidget(title);

    // KPI kartları
    auto* kpiRow = new QHBoxLayout;
    kpiRow->setSpacing(24);
    kpiRow->addWidget(makeCard("TOPLAM BYTE", m_totalVal));
    kpiRow->addWidget(makeCard("BENZERSİZ OPCODE", m_uniqueVal));
    kpiRow->addWidget(makeCard("SHANNON ENTROPİ", m_entropyVal));
    kpiRow->addWidget(makeCard("EN SIK OPCODE", m_mostFreqVal));
    root->addLayout(kpiRow);

    // Kategori özeti
    m_catLabel = new QLabel(this);
    m_catLabel->setWordWrap(true);
    m_catLabel->setObjectName("kpiLabel");
    root->addWidget(m_catLabel);

    root->addStretch();
    clear();
}

QFrame* SummaryView::makeCard(const QString& title, QLabel*& valueLabel) {
    auto* card = new QFrame;
    card->setObjectName("kpiCard");
    card->setMinimumWidth(200);
    card->setMinimumHeight(140);

    auto* lay = new QVBoxLayout(card);
    lay->setContentsMargins(30, 25, 30, 25);
    lay->setSpacing(10);

    valueLabel = new QLabel("—", card);
    valueLabel->setObjectName("kpiValue");
    valueLabel->setAlignment(Qt::AlignCenter);

    auto* lbl = new QLabel(title, card);
    lbl->setObjectName("kpiLabel");
    lbl->setAlignment(Qt::AlignCenter);

    lay->addWidget(valueLabel);
    lay->addWidget(lbl);
    return card;
}

void SummaryView::setReport(const Report& r) {
    m_totalVal->setText(QString::number(r.totalInstructions));
    m_uniqueVal->setText(QString::number(r.uniqueOpcodes));

    std::ostringstream ss;
    ss << std::fixed << std::setprecision(4) << r.entropy;
    m_entropyVal->setText(QString::fromStdString(ss.str()));

    m_mostFreqVal->setText(r.mostFrequent.empty() ? "—"
                                                   : QString::fromStdString(r.mostFrequent));

    // Kategori dağılımı metni
    QString catText = "Kategori Dağılımı:  ";
    for (const auto& [cat, cnt] : r.categoryFreq) {
        double pct = r.totalInstructions > 0
                     ? 100.0 * cnt / r.totalInstructions : 0.0;
        catText += QString("  %1: %2 (%3%)").arg(QString::fromStdString(cat))
                       .arg(cnt).arg(pct, 0, 'f', 1);
    }
    m_catLabel->setText(catText);
}

void SummaryView::clear() {
    m_totalVal->setText("—");
    m_uniqueVal->setText("—");
    m_entropyVal->setText("—");
    m_mostFreqVal->setText("—");
    m_catLabel->setText("Analiz için herhangi bir binary dosyayı (.exe .dll .bin .elf) açın.");
}
