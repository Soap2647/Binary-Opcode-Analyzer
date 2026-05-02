#pragma once
#include <cstdint>
#include <string>
#include <ostream>

// Soyut taban sınıf — artık tek bir opcode byte saklar.
// reg1/reg2/imm kaldırıldı; x86'da operandlar ModRM byte'ından gelir
// ancak biz burada sadece opcode-level analiz yapıyoruz.
class Instruction {
public:
    explicit Instruction(uint8_t opcode) : m_opcode(opcode) {}
    virtual ~Instruction() = default;

    virtual std::string name()        const = 0;
    virtual std::string category()    const = 0;
    virtual std::string description() const = 0;

    // Operandlar bu seviyede gösterilmiyor (ModRM gerektiriyor)
    virtual std::string operands() const { return ""; }

    uint8_t opcode() const { return m_opcode; }

    std::string toString() const { return name(); }
    friend std::ostream& operator<<(std::ostream& os, const Instruction& i)
    { return os << i.toString(); }

protected:
    uint8_t m_opcode;
};
