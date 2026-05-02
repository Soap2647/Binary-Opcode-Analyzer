#pragma once
#include "Analyzer.h"

class ComparisonAnalyzer : public Analyzer {
public:
    // Tek program analizi (Analyzer arayüzünü karşılar)
    Report      analyze(const Program& program) const override;
    std::string name()  const override { return "Karşılaştırma Analizörü"; }

    // İki raporu karşılaştırır ve diff bilgisi dolu bir rapor döner
    static Report compare(const Report& a, const Report& b);
};
