#include "InstructionFactory.h"
#include "GenericInstruction.h"

std::unique_ptr<Instruction> InstructionFactory::create(uint8_t opcode) {
    return std::make_unique<GenericInstruction>(opcode);
}
