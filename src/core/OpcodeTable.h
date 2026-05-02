#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>

// x86 opcode kategorileri
enum class OpcodeCategory {
    DataTransfer,   // MOV, PUSH, POP, XCHG, LEA, ...
    Arithmetic,     // ADD, SUB, MUL, DIV, INC, DEC, NEG, ...
    Logic,          // AND, OR, XOR, NOT, TEST, ...
    ControlFlow,    // JMP, Jcc, CALL, RET, LOOP, ...
    String,         // MOVS, STOS, LODS, CMPS, SCAS, REP, ...
    BitShift,       // SHL, SHR, SAR, ROL, ROR, ...
    System,         // INT, SYSCALL, IN, OUT, HLT, NOP, CPUID, ...
    FPU,            // x87 ESC bytes (0xD8-0xDF)
    Prefix,         // Segment/lock/rep prefix bytes
    Unknown
};

// Tek-byte x86 opcode bilgisi
struct OpcodeInfo {
    std::string    mnemonic;
    OpcodeCategory category;
    std::string    description;
};

class OpcodeTable {
public:
    static OpcodeTable& instance();

    const OpcodeInfo* lookup(uint8_t opcode) const;
    std::string       mnemonic(uint8_t opcode)     const;
    OpcodeCategory    category(uint8_t opcode)     const;
    std::string       categoryName(OpcodeCategory) const;

    const std::unordered_map<uint8_t, OpcodeInfo>& all() const { return m_table; }

private:
    OpcodeTable();
    std::unordered_map<uint8_t, OpcodeInfo> m_table;
};
