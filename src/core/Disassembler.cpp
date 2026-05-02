#include "Disassembler.h"
#include "InstructionFactory.h"

Program Disassembler::disassemble(const BinaryFile& file) const {
    Program prog(file.path());
    for (uint8_t byte : file.bytes()) {
        prog.add(InstructionFactory::create(byte));
    }
    return prog;
}

Program Disassembler::disassemble(const std::string& path) const {
    BinaryFile file(path); // fırlatabilir
    return disassemble(file);
}
