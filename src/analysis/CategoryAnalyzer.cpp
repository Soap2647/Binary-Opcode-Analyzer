#include "CategoryAnalyzer.h"

Report CategoryAnalyzer::analyze(const Program& program) const {
    Report r;
    r.filename          = program.filename();
    r.totalInstructions = static_cast<uint32_t>(program.size());

    for (const auto& instr : program.instructions()) {
        r.categoryFreq[instr->category()]++;
        r.opcodeFreq[instr->name()].count++;
    }

    if (r.totalInstructions > 0) {
        for (auto& [mnem, stats] : r.opcodeFreq)
            stats.percentage = 100.0 * stats.count / r.totalInstructions;
    }

    r.uniqueOpcodes = static_cast<uint32_t>(r.opcodeFreq.size());
    return r;
}
