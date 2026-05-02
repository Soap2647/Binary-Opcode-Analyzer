#pragma once
#include "Instruction.h"
#include "OpcodeTable.h"
#include <string>

// Tüm x86 opcode'ları için genel instruction sınıfı.
// OpcodeTable'dan mnemonic, kategori ve açıklamayı alır.
class GenericInstruction : public Instruction {
public:
    explicit GenericInstruction(uint8_t opcode)
        : Instruction(opcode) {}

    std::string name() const override {
        return OpcodeTable::instance().mnemonic(m_opcode);
    }

    std::string category() const override {
        return OpcodeTable::instance().categoryName(
            OpcodeTable::instance().category(m_opcode));
    }

    std::string description() const override {
        const auto* info = OpcodeTable::instance().lookup(m_opcode);
        return info ? info->description : "Bilinmeyen opcode";
    }
};
