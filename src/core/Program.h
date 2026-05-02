#pragma once
#include "Instruction.h"
#include <vector>
#include <memory>
#include <string>

// Program: bir binary dosyadan yüklenen talimat dizisini saklar.
// Composition + RAII: Instruction nesnelerini unique_ptr ile sahiplenir.
class Program {
public:
    Program() = default;
    explicit Program(const std::string& filename) : m_filename(filename) {}

    void add(std::unique_ptr<Instruction> instr);

    const std::vector<std::unique_ptr<Instruction>>& instructions() const { return m_instructions; }
    size_t size()  const { return m_instructions.size(); }
    bool   empty() const { return m_instructions.empty(); }

    const Instruction* at(size_t idx) const;

    const std::string& filename() const { return m_filename; }
    void setFilename(const std::string& fn) { m_filename = fn; }

    void clear();

private:
    std::string m_filename;
    std::vector<std::unique_ptr<Instruction>> m_instructions;
};
