#pragma once
#include "Analyzer.h"

class FrequencyAnalyzer : public Analyzer {
public:
    Report      analyze(const Program& program) const override;
    std::string name()  const override { return "Frekans Analizörü"; }
};
