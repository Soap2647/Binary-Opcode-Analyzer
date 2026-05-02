#pragma once
#include "Program.h"
#include "BinaryFile.h"
#include <string>

// Disassembler: BinaryFile'daki ham byte'ları Program nesnesine dönüştürür
class Disassembler {
public:
    Program disassemble(const BinaryFile& file) const;
    Program disassemble(const std::string& path) const;
};
