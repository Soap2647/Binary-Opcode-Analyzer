#include "OpcodeTable.h"

OpcodeTable& OpcodeTable::instance() {
    static OpcodeTable inst;
    return inst;
}

OpcodeTable::OpcodeTable() {
    using C = OpcodeCategory;

    // ── Prefix bytes ─────────────────────────────────────────────────────────
    m_table[0x26] = {"ES:",    C::Prefix,      "ES segment prefix"};
    m_table[0x2E] = {"CS:",    C::Prefix,      "CS segment prefix"};
    m_table[0x36] = {"SS:",    C::Prefix,      "SS segment prefix"};
    m_table[0x3E] = {"DS:",    C::Prefix,      "DS segment prefix"};
    m_table[0x64] = {"FS:",    C::Prefix,      "FS segment prefix"};
    m_table[0x65] = {"GS:",    C::Prefix,      "GS segment prefix"};
    m_table[0x66] = {"OPDSZ", C::Prefix,      "Operand size prefix"};
    m_table[0x67] = {"ADRSZ", C::Prefix,      "Address size prefix"};
    m_table[0xF0] = {"LOCK",  C::Prefix,      "LOCK prefix"};
    m_table[0xF2] = {"REPNE", C::Prefix,      "REPNE/REPNZ prefix"};
    m_table[0xF3] = {"REP",   C::Prefix,      "REP/REPE prefix"};
    m_table[0x0F] = {"ESC",   C::Prefix,      "Two-byte opcode escape"};

    // ── Data Transfer ─────────────────────────────────────────────────────────
    // MOV variants 0x88-0x8B, 0xA0-0xA3, 0xB0-0xBF, 0xC6-0xC7
    for (uint8_t b = 0x88; b <= 0x8B; ++b)
        m_table[b] = {"MOV", C::DataTransfer, "Move data between r/m and reg"};
    m_table[0x8C] = {"MOV", C::DataTransfer, "Move segment register to r/m"};
    m_table[0x8E] = {"MOV", C::DataTransfer, "Move r/m to segment register"};
    m_table[0xA0] = {"MOV", C::DataTransfer, "Move memory byte to AL"};
    m_table[0xA1] = {"MOV", C::DataTransfer, "Move memory word to AX/EAX/RAX"};
    m_table[0xA2] = {"MOV", C::DataTransfer, "Move AL to memory byte"};
    m_table[0xA3] = {"MOV", C::DataTransfer, "Move AX/EAX/RAX to memory"};
    for (uint8_t b = 0xB0; b <= 0xB7; ++b)
        m_table[b] = {"MOV", C::DataTransfer, "Move imm8 to r8"};
    for (uint8_t b = 0xB8; b <= 0xBF; ++b)
        m_table[b] = {"MOV", C::DataTransfer, "Move imm to r16/r32/r64"};
    m_table[0xC6] = {"MOV", C::DataTransfer, "Move imm8 to r/m8"};
    m_table[0xC7] = {"MOV", C::DataTransfer, "Move imm to r/m"};

    // PUSH
    for (uint8_t b = 0x50; b <= 0x57; ++b)
        m_table[b] = {"PUSH", C::DataTransfer, "Push register onto stack"};
    m_table[0x68] = {"PUSH", C::DataTransfer, "Push imm onto stack"};
    m_table[0x6A] = {"PUSH", C::DataTransfer, "Push imm8 sign-extended onto stack"};
    m_table[0xFF] = {"PUSH/JMP/CALL", C::DataTransfer, "PUSH/INC/DEC/CALL/JMP group (ModRM)"};
    m_table[0x8F] = {"POP",  C::DataTransfer, "Pop r/m from stack"};

    // POP
    for (uint8_t b = 0x58; b <= 0x5F; ++b)
        m_table[b] = {"POP",  C::DataTransfer, "Pop stack into register"};

    // XCHG
    for (uint8_t b = 0x90; b <= 0x97; ++b)
        m_table[b] = {b == 0x90 ? "NOP" : "XCHG",
                      b == 0x90 ? C::System : C::DataTransfer,
                      b == 0x90 ? "No operation" : "Exchange register with AX/EAX/RAX"};

    // LEA
    m_table[0x8D] = {"LEA", C::DataTransfer, "Load effective address"};

    // MOVS / LODS / STOS / CMPS / SCAS
    m_table[0xA4] = {"MOVSB", C::String, "Move byte string"};
    m_table[0xA5] = {"MOVSW", C::String, "Move word/dword/qword string"};
    m_table[0xA6] = {"CMPSB", C::String, "Compare byte strings"};
    m_table[0xA7] = {"CMPSW", C::String, "Compare word/dword strings"};
    m_table[0xA8] = {"TEST",  C::Logic,  "Logical compare AL with imm8"};
    m_table[0xA9] = {"TEST",  C::Logic,  "Logical compare AX/EAX/RAX with imm"};
    m_table[0xAA] = {"STOSB", C::String, "Store byte in string"};
    m_table[0xAB] = {"STOSW", C::String, "Store word/dword/qword in string"};
    m_table[0xAC] = {"LODSB", C::String, "Load byte from string"};
    m_table[0xAD] = {"LODSW", C::String, "Load word/dword/qword from string"};
    m_table[0xAE] = {"SCASB", C::String, "Scan byte string"};
    m_table[0xAF] = {"SCASW", C::String, "Scan word/dword string"};

    // XLATB
    m_table[0xD7] = {"XLATB", C::DataTransfer, "Table look-up translation"};

    // PUSHF / POPF
    m_table[0x9C] = {"PUSHF", C::DataTransfer, "Push EFLAGS onto stack"};
    m_table[0x9D] = {"POPF",  C::DataTransfer, "Pop EFLAGS from stack"};
    m_table[0x60] = {"PUSHA", C::DataTransfer, "Push all general registers"};
    m_table[0x61] = {"POPA",  C::DataTransfer, "Pop all general registers"};

    // ── Arithmetic ────────────────────────────────────────────────────────────
    // ADD
    m_table[0x00] = {"ADD", C::Arithmetic, "Add r8 to r/m8"};
    m_table[0x01] = {"ADD", C::Arithmetic, "Add r to r/m"};
    m_table[0x02] = {"ADD", C::Arithmetic, "Add r/m8 to r8"};
    m_table[0x03] = {"ADD", C::Arithmetic, "Add r/m to r"};
    m_table[0x04] = {"ADD", C::Arithmetic, "Add imm8 to AL"};
    m_table[0x05] = {"ADD", C::Arithmetic, "Add imm to AX/EAX/RAX"};
    // ADC
    m_table[0x10] = {"ADC", C::Arithmetic, "Add with carry r8 to r/m8"};
    m_table[0x11] = {"ADC", C::Arithmetic, "Add with carry r to r/m"};
    m_table[0x12] = {"ADC", C::Arithmetic, "Add with carry r/m8 to r8"};
    m_table[0x13] = {"ADC", C::Arithmetic, "Add with carry r/m to r"};
    m_table[0x14] = {"ADC", C::Arithmetic, "Add with carry imm8 to AL"};
    m_table[0x15] = {"ADC", C::Arithmetic, "Add with carry imm to AX/EAX/RAX"};
    // SUB
    m_table[0x28] = {"SUB", C::Arithmetic, "Subtract r8 from r/m8"};
    m_table[0x29] = {"SUB", C::Arithmetic, "Subtract r from r/m"};
    m_table[0x2A] = {"SUB", C::Arithmetic, "Subtract r/m8 from r8"};
    m_table[0x2B] = {"SUB", C::Arithmetic, "Subtract r/m from r"};
    m_table[0x2C] = {"SUB", C::Arithmetic, "Subtract imm8 from AL"};
    m_table[0x2D] = {"SUB", C::Arithmetic, "Subtract imm from AX/EAX/RAX"};
    // SBB
    m_table[0x18] = {"SBB", C::Arithmetic, "Subtract with borrow r8 from r/m8"};
    m_table[0x19] = {"SBB", C::Arithmetic, "Subtract with borrow r from r/m"};
    m_table[0x1A] = {"SBB", C::Arithmetic, "Subtract with borrow r/m8 from r8"};
    m_table[0x1B] = {"SBB", C::Arithmetic, "Subtract with borrow r/m from r"};
    m_table[0x1C] = {"SBB", C::Arithmetic, "Subtract with borrow imm8 from AL"};
    m_table[0x1D] = {"SBB", C::Arithmetic, "Subtract with borrow imm from AX/EAX/RAX"};
    // INC
    for (uint8_t b = 0x40; b <= 0x47; ++b)
        m_table[b] = {"INC", C::Arithmetic, "Increment register by 1"};
    // DEC
    for (uint8_t b = 0x48; b <= 0x4F; ++b)
        m_table[b] = {"DEC", C::Arithmetic, "Decrement register by 1"};
    // MUL / IMUL / DIV / IDIV / NEG via group opcode
    m_table[0xF6] = {"MUL/IMUL/DIV/IDIV/NEG/NOT", C::Arithmetic, "Arithmetic group r/m8 (ModRM)"};
    m_table[0xF7] = {"MUL/IMUL/DIV/IDIV/NEG/NOT", C::Arithmetic, "Arithmetic group r/m (ModRM)"};
    // IMUL 2-operand
    m_table[0x69] = {"IMUL", C::Arithmetic, "Signed multiply r/m by imm"};
    m_table[0x6B] = {"IMUL", C::Arithmetic, "Signed multiply r/m by imm8"};
    // CMP
    m_table[0x38] = {"CMP", C::Arithmetic, "Compare r8 with r/m8"};
    m_table[0x39] = {"CMP", C::Arithmetic, "Compare r with r/m"};
    m_table[0x3A] = {"CMP", C::Arithmetic, "Compare r/m8 with r8"};
    m_table[0x3B] = {"CMP", C::Arithmetic, "Compare r/m with r"};
    m_table[0x3C] = {"CMP", C::Arithmetic, "Compare imm8 with AL"};
    m_table[0x3D] = {"CMP", C::Arithmetic, "Compare imm with AX/EAX/RAX"};
    // DAA / DAS / AAA / AAS
    m_table[0x27] = {"DAA", C::Arithmetic, "Decimal adjust AL after addition"};
    m_table[0x2F] = {"DAS", C::Arithmetic, "Decimal adjust AL after subtraction"};
    m_table[0x37] = {"AAA", C::Arithmetic, "ASCII adjust AL after addition"};
    m_table[0x3F] = {"AAS", C::Arithmetic, "ASCII adjust AL after subtraction"};
    // ADD/SUB/CMP imm group
    m_table[0x80] = {"ADD/OR/ADC/SBB/AND/SUB/XOR/CMP", C::Arithmetic, "Immediate group r/m8 (ModRM)"};
    m_table[0x81] = {"ADD/OR/ADC/SBB/AND/SUB/XOR/CMP", C::Arithmetic, "Immediate group r/m (ModRM)"};
    m_table[0x82] = {"ADD/OR/ADC/SBB/AND/SUB/XOR/CMP", C::Arithmetic, "Immediate group r/m8 sign-ext (ModRM)"};
    m_table[0x83] = {"ADD/OR/ADC/SBB/AND/SUB/XOR/CMP", C::Arithmetic, "Immediate group r/m sign-ext imm8 (ModRM)"};

    // ── Logic ─────────────────────────────────────────────────────────────────
    // AND
    m_table[0x20] = {"AND", C::Logic, "AND r8 with r/m8"};
    m_table[0x21] = {"AND", C::Logic, "AND r with r/m"};
    m_table[0x22] = {"AND", C::Logic, "AND r/m8 with r8"};
    m_table[0x23] = {"AND", C::Logic, "AND r/m with r"};
    m_table[0x24] = {"AND", C::Logic, "AND imm8 with AL"};
    m_table[0x25] = {"AND", C::Logic, "AND imm with AX/EAX/RAX"};
    // OR
    m_table[0x08] = {"OR",  C::Logic, "OR r8 with r/m8"};
    m_table[0x09] = {"OR",  C::Logic, "OR r with r/m"};
    m_table[0x0A] = {"OR",  C::Logic, "OR r/m8 with r8"};
    m_table[0x0B] = {"OR",  C::Logic, "OR r/m with r"};
    m_table[0x0C] = {"OR",  C::Logic, "OR imm8 with AL"};
    m_table[0x0D] = {"OR",  C::Logic, "OR imm with AX/EAX/RAX"};
    // XOR
    m_table[0x30] = {"XOR", C::Logic, "XOR r8 with r/m8"};
    m_table[0x31] = {"XOR", C::Logic, "XOR r with r/m"};
    m_table[0x32] = {"XOR", C::Logic, "XOR r/m8 with r8"};
    m_table[0x33] = {"XOR", C::Logic, "XOR r/m with r"};
    m_table[0x34] = {"XOR", C::Logic, "XOR imm8 with AL"};
    m_table[0x35] = {"XOR", C::Logic, "XOR imm with AX/EAX/RAX"};
    // TEST
    m_table[0x84] = {"TEST", C::Logic, "AND r8 with r/m8, set flags"};
    m_table[0x85] = {"TEST", C::Logic, "AND r with r/m, set flags"};
    // NOT / NEG in F6/F7 group (already added)
    // SHR/SHL/SAR/ROL/ROR group
    m_table[0xC0] = {"SHL/SHR/SAR/ROL/ROR", C::BitShift, "Shift/rotate r/m8 by imm8 (ModRM)"};
    m_table[0xC1] = {"SHL/SHR/SAR/ROL/ROR", C::BitShift, "Shift/rotate r/m by imm8 (ModRM)"};
    m_table[0xD0] = {"SHL/SHR/SAR/ROL/ROR", C::BitShift, "Shift/rotate r/m8 by 1 (ModRM)"};
    m_table[0xD1] = {"SHL/SHR/SAR/ROL/ROR", C::BitShift, "Shift/rotate r/m by 1 (ModRM)"};
    m_table[0xD2] = {"SHL/SHR/SAR/ROL/ROR", C::BitShift, "Shift/rotate r/m8 by CL (ModRM)"};
    m_table[0xD3] = {"SHL/SHR/SAR/ROL/ROR", C::BitShift, "Shift/rotate r/m by CL (ModRM)"};

    // ── Control Flow ──────────────────────────────────────────────────────────
    // JMP
    m_table[0xEB] = {"JMP",   C::ControlFlow, "Short jump rel8"};
    m_table[0xE9] = {"JMP",   C::ControlFlow, "Near jump rel16/32"};
    m_table[0xEA] = {"JMP",   C::ControlFlow, "Far jump ptr16:16/32"};
    // Jcc short
    m_table[0x70] = {"JO",    C::ControlFlow, "Jump if overflow"};
    m_table[0x71] = {"JNO",   C::ControlFlow, "Jump if not overflow"};
    m_table[0x72] = {"JB",    C::ControlFlow, "Jump if below/carry"};
    m_table[0x73] = {"JAE",   C::ControlFlow, "Jump if above or equal"};
    m_table[0x74] = {"JE",    C::ControlFlow, "Jump if equal/zero"};
    m_table[0x75] = {"JNE",   C::ControlFlow, "Jump if not equal/zero"};
    m_table[0x76] = {"JBE",   C::ControlFlow, "Jump if below or equal"};
    m_table[0x77] = {"JA",    C::ControlFlow, "Jump if above"};
    m_table[0x78] = {"JS",    C::ControlFlow, "Jump if sign"};
    m_table[0x79] = {"JNS",   C::ControlFlow, "Jump if not sign"};
    m_table[0x7A] = {"JP",    C::ControlFlow, "Jump if parity even"};
    m_table[0x7B] = {"JNP",   C::ControlFlow, "Jump if parity odd"};
    m_table[0x7C] = {"JL",    C::ControlFlow, "Jump if less"};
    m_table[0x7D] = {"JGE",   C::ControlFlow, "Jump if greater or equal"};
    m_table[0x7E] = {"JLE",   C::ControlFlow, "Jump if less or equal"};
    m_table[0x7F] = {"JG",    C::ControlFlow, "Jump if greater"};
    // CALL / RET
    m_table[0xE8] = {"CALL",  C::ControlFlow, "Call near procedure"};
    m_table[0x9A] = {"CALL",  C::ControlFlow, "Call far procedure"};
    m_table[0xC3] = {"RET",   C::ControlFlow, "Return from near procedure"};
    m_table[0xC2] = {"RET",   C::ControlFlow, "Return from near procedure, pop imm16"};
    m_table[0xCB] = {"RETF",  C::ControlFlow, "Return from far procedure"};
    m_table[0xCA] = {"RETF",  C::ControlFlow, "Return from far procedure, pop imm16"};
    // LOOP
    m_table[0xE0] = {"LOOPNE",C::ControlFlow, "Decrement ECX, jump if ECX!=0 and ZF=0"};
    m_table[0xE1] = {"LOOPE", C::ControlFlow, "Decrement ECX, jump if ECX!=0 and ZF=1"};
    m_table[0xE2] = {"LOOP",  C::ControlFlow, "Decrement ECX, jump if ECX != 0"};
    m_table[0xE3] = {"JECXZ", C::ControlFlow, "Jump if ECX is zero"};
    // ENTER / LEAVE
    m_table[0xC8] = {"ENTER", C::ControlFlow, "Make stack frame for procedure"};
    m_table[0xC9] = {"LEAVE", C::ControlFlow, "Restore stack frame"};

    // ── String ────────────────────────────────────────────────────────────────
    // (already added above A4-AF)

    // ── System / Misc ─────────────────────────────────────────────────────────
    m_table[0xCC] = {"INT3",  C::System,  "Breakpoint interrupt"};
    m_table[0xCD] = {"INT",   C::System,  "Software interrupt"};
    m_table[0xCE] = {"INTO",  C::System,  "Interrupt if overflow"};
    m_table[0xCF] = {"IRET",  C::System,  "Return from interrupt"};
    m_table[0xF4] = {"HLT",   C::System,  "Halt processor"};
    m_table[0xF5] = {"CMC",   C::System,  "Complement carry flag"};
    m_table[0xF8] = {"CLC",   C::System,  "Clear carry flag"};
    m_table[0xF9] = {"STC",   C::System,  "Set carry flag"};
    m_table[0xFA] = {"CLI",   C::System,  "Clear interrupt flag"};
    m_table[0xFB] = {"STI",   C::System,  "Set interrupt flag"};
    m_table[0xFC] = {"CLD",   C::System,  "Clear direction flag"};
    m_table[0xFD] = {"STD",   C::System,  "Set direction flag"};
    m_table[0x98] = {"CWDE",  C::System,  "Convert word to dword (sign-extend AL->AX)"};
    m_table[0x99] = {"CDQ",   C::System,  "Convert dword to qword (sign-extend EAX->EDX:EAX)"};
    m_table[0x9B] = {"WAIT",  C::System,  "Wait for FPU"};
    m_table[0x9E] = {"SAHF",  C::System,  "Store AH into flags"};
    m_table[0x9F] = {"LAHF",  C::System,  "Load AH from flags"};
    m_table[0xD4] = {"AAM",   C::System,  "ASCII adjust AX after multiply"};
    m_table[0xD5] = {"AAD",   C::System,  "ASCII adjust AX before division"};
    m_table[0xD6] = {"SALC",  C::System,  "Set AL if carry (undocumented)"};
    // IN / OUT
    m_table[0xE4] = {"IN",    C::System,  "Input byte from port imm8 to AL"};
    m_table[0xE5] = {"IN",    C::System,  "Input word/dword from port imm8 to AX/EAX"};
    m_table[0xE6] = {"OUT",   C::System,  "Output AL to port imm8"};
    m_table[0xE7] = {"OUT",   C::System,  "Output AX/EAX to port imm8"};
    m_table[0xEC] = {"IN",    C::System,  "Input byte from port DX to AL"};
    m_table[0xED] = {"IN",    C::System,  "Input word/dword from port DX to AX/EAX"};
    m_table[0xEE] = {"OUT",   C::System,  "Output AL to port DX"};
    m_table[0xEF] = {"OUT",   C::System,  "Output AX/EAX to port DX"};
    // LES / LDS / BOUND
    m_table[0xC4] = {"LES",   C::DataTransfer, "Load ES and r with far ptr"};
    m_table[0xC5] = {"LDS",   C::DataTransfer, "Load DS and r with far ptr"};
    m_table[0x62] = {"BOUND", C::System,  "Check array index against bounds"};
    m_table[0x63] = {"ARPL",  C::System,  "Adjust RPL field of selector"};
    // FPU (x87 escape bytes)
    for (uint8_t b = 0xD8; b <= 0xDF; ++b)
        m_table[b] = {"FPU", C::FPU, "x87 FPU instruction group"};
    // FE group: INC/DEC r/m8
    m_table[0xFE] = {"INC/DEC", C::Arithmetic, "Increment/Decrement r/m8 (ModRM)"};
    // 8C/8E already added; 8D = LEA
    m_table[0x86] = {"XCHG", C::DataTransfer, "Exchange r8 with r/m8"};
    m_table[0x87] = {"XCHG", C::DataTransfer, "Exchange r with r/m"};
    m_table[0x6C] = {"INSB",  C::String,  "Input byte from port DX to ES:EDI"};
    m_table[0x6D] = {"INSW",  C::String,  "Input word/dword from port DX to ES:EDI"};
    m_table[0x6E] = {"OUTSB", C::String,  "Output byte from DS:ESI to port DX"};
    m_table[0x6F] = {"OUTSW", C::String,  "Output word/dword from DS:ESI to port DX"};
    // MOVZX/MOVSX partially via 0F — here the 0F escape byte is already marked Prefix
}

const OpcodeInfo* OpcodeTable::lookup(uint8_t opcode) const {
    auto it = m_table.find(opcode);
    return (it != m_table.end()) ? &it->second : nullptr;
}

std::string OpcodeTable::mnemonic(uint8_t opcode) const {
    const auto* info = lookup(opcode);
    return info ? info->mnemonic : "???";
}

OpcodeCategory OpcodeTable::category(uint8_t opcode) const {
    const auto* info = lookup(opcode);
    return info ? info->category : OpcodeCategory::Unknown;
}

std::string OpcodeTable::categoryName(OpcodeCategory cat) const {
    switch (cat) {
        case OpcodeCategory::DataTransfer: return "Veri Transferi";
        case OpcodeCategory::Arithmetic:   return "Aritmetik";
        case OpcodeCategory::Logic:        return "Mantıksal";
        case OpcodeCategory::ControlFlow:  return "Kontrol Akışı";
        case OpcodeCategory::String:       return "String";
        case OpcodeCategory::BitShift:     return "Bit Kaydırma";
        case OpcodeCategory::System:       return "Sistem";
        case OpcodeCategory::FPU:          return "FPU";
        case OpcodeCategory::Prefix:       return "Prefix";
        default:                           return "Bilinmeyen";
    }
}
