#pragma once
#include "Report.h"
#include "core/Program.h"

// Strategy Pattern: her Analyzer alt sınıfı farklı bir analiz stratejisi uygular
class Analyzer {
public:
    virtual ~Analyzer() = default;
    virtual Report analyze(const Program& program) const = 0;
    virtual std::string name() const = 0;
};
