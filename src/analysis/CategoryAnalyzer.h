#pragma once
#include "Analyzer.h"

class CategoryAnalyzer : public Analyzer {
public:
    Report      analyze(const Program& program) const override;
    std::string name()  const override { return "Kategori Analizörü"; }
};
