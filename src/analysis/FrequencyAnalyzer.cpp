#include "FrequencyAnalyzer.h"
#include <algorithm>
#include <filesystem>

Report FrequencyAnalyzer::analyze(const Program& program) const {
    Report r;
    r.filename = program.filename();
    r.totalInstructions = static_cast<uint32_t>(program.size());

    if (program.empty()) return r;

    // Opcode sayımı
    for (const auto& instr : program.instructions()) {
        r.opcodeFreq[instr->name()].count++;
        r.categoryFreq[instr->category()]++;
    }

    // Yüzde hesaplama
    for (auto& [mnem, stats] : r.opcodeFreq) {
        stats.percentage = 100.0 * stats.count / r.totalInstructions;
    }

    r.uniqueOpcodes = static_cast<uint32_t>(r.opcodeFreq.size());

    // En sık
    auto it = std::max_element(r.opcodeFreq.begin(), r.opcodeFreq.end(),
        [](const auto& a, const auto& b){ return a.second.count < b.second.count; });
    r.mostFrequent = it->first;

    return r;
}
