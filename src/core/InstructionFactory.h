#pragma once
#include "Instruction.h"
#include <memory>
#include <cstdint>

// Factory Pattern: ham opcode byte'tan Instruction nesnesi üretir.
class InstructionFactory {
public:
    static std::unique_ptr<Instruction> create(uint8_t opcode);
};
