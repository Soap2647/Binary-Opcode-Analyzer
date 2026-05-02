#include "Program.h"

void Program::add(std::unique_ptr<Instruction> instr) {
    m_instructions.push_back(std::move(instr));
}

const Instruction* Program::at(size_t idx) const {
    if (idx < m_instructions.size())
        return m_instructions[idx].get();
    return nullptr;
}

void Program::clear() {
    m_instructions.clear();
    m_filename.clear();
}
