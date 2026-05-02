#pragma once
#include "Analyzer.h"

// Shannon entropisini hesaplar: H = -Σ p(x) * log2(p(x))
// Yüksek entropi → çeşitli / rastgele kod; Düşük entropi → tekrarlayan desen
class EntropyAnalyzer : public Analyzer {
public:
    Report      analyze(const Program& program) const override;
    std::string name()  const override { return "Entropi Analizörü"; }

    static double shannonEntropy(const Program& program);
};
