#include "EntropyAnalyzer.h"
#include "FrequencyAnalyzer.h"
#include <cmath>

double EntropyAnalyzer::shannonEntropy(const Program& program) {
    if (program.empty()) return 0.0;

    FrequencyAnalyzer freq;
    Report r = freq.analyze(program);

    double H = 0.0;
    for (const auto& [mnem, stats] : r.opcodeFreq) {
        double p = stats.percentage / 100.0;
        if (p > 0.0)
            H -= p * std::log2(p);
    }
    return H;
}

Report EntropyAnalyzer::analyze(const Program& program) const {
    FrequencyAnalyzer freq;
    Report r = freq.analyze(program);
    r.entropy = shannonEntropy(program);
    return r;
}
