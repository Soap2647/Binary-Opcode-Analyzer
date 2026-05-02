#include "ComparisonAnalyzer.h"
#include "FrequencyAnalyzer.h"
#include <set>
#include <cmath>

Report ComparisonAnalyzer::analyze(const Program& program) const {
    FrequencyAnalyzer freq;
    return freq.analyze(program);
}

Report ComparisonAnalyzer::compare(const Report& a, const Report& b) {
    Report result;
    result.isComparison = true;
    result.filename     = a.filename;
    result.filename2    = b.filename;

    // Tüm opcode'ları topla
    std::set<std::string> allMnemonics;
    for (const auto& [m, _] : a.opcodeFreq) allMnemonics.insert(m);
    for (const auto& [m, _] : b.opcodeFreq) allMnemonics.insert(m);

    for (const auto& mnem : allMnemonics) {
        uint32_t c1 = 0, c2 = 0;
        if (a.opcodeFreq.count(mnem)) c1 = a.opcodeFreq.at(mnem).count;
        if (b.opcodeFreq.count(mnem)) c2 = b.opcodeFreq.at(mnem).count;

        OpcodeDiff diff;
        diff.mnemonic   = mnem;
        diff.count1     = c1;
        diff.count2     = c2;
        diff.difference = static_cast<int32_t>(c2) - static_cast<int32_t>(c1);
        diff.changePercent = (c1 > 0) ? (100.0 * diff.difference / c1) : (c2 > 0 ? 100.0 : 0.0);

        result.diffs.push_back(diff);

        result.opcodeFreq[mnem].count = c1 + c2;
    }

    result.totalInstructions = a.totalInstructions + b.totalInstructions;
    result.uniqueOpcodes     = static_cast<uint32_t>(allMnemonics.size());

    return result;
}
