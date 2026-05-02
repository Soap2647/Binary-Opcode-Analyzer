// opcgen.cpp — Örnek .opc dosyaları üreten CLI aracı
// Kullanım: opcgen <hedef_klasör>
//   hello.opc   — Basit bir "merhaba dünya" programı
//   loop.opc    — Sayaç döngüsü (düşük entropi)
//   packed.opc  — Rastgele talimatlar (yüksek entropi)

#include <fstream>
#include <vector>
#include <cstdint>
#include <string>
#include <iostream>
#include <random>
#include <cstring>

static constexpr uint32_t MAGIC   = 0x4F504331;
static constexpr uint16_t VERSION = 1;

struct Instr { uint8_t op, r1, r2, imm; };

// Opcode sabitleri (OpcodeTable.h ile aynı)
namespace Op {
    constexpr uint8_t ADD=0x01,SUB=0x02,MUL=0x03,DIV=0x04,MOD=0x05,INC=0x06,DEC=0x07;
    constexpr uint8_t LOAD=0x10,STORE=0x11,MOV=0x12,PUSH=0x13,POP=0x14;
    constexpr uint8_t JMP=0x20,JEQ=0x21,JNE=0x22,JLT=0x23,JGT=0x24,CALL=0x25,RET=0x26,HALT=0x27;
    constexpr uint8_t AND=0x30,OR=0x31,XOR=0x32,NOT=0x33,CMP=0x34;
    constexpr uint8_t IN=0x40,OUT=0x41;
}

static bool write(const std::string& path, const std::vector<Instr>& instrs) {
    std::ofstream f(path, std::ios::binary);
    if (!f) { std::cerr << "Hata: " << path << " yazılamadı\n"; return false; }

    uint32_t count = static_cast<uint32_t>(instrs.size());
    f.write(reinterpret_cast<const char*>(&MAGIC),   4);
    f.write(reinterpret_cast<const char*>(&VERSION), 2);
    f.write(reinterpret_cast<const char*>(&count),   4);
    for (const auto& i : instrs)
        f.write(reinterpret_cast<const char*>(&i), 4);

    std::cout << "  Oluşturuldu: " << path
              << "  (" << instrs.size() << " talimat)\n";
    return true;
}

// ── Örnek 1: Basit "merhaba dünya" programı ──────────────────────────────────
static std::vector<Instr> makeHello() {
    return {
        {Op::MOV,  0,1,0},   // MOV R0, R1
        {Op::LOAD, 0,0,10},  // LOAD R0, #10
        {Op::LOAD, 1,0,20},  // LOAD R1, #20
        {Op::ADD,  0,1,0},   // ADD R0, R1
        {Op::MOV,  2,0,0},   // MOV R2, R0
        {Op::OUT,  2,0,0},   // OUT R2
        {Op::PUSH, 0,0,0},   // PUSH R0
        {Op::POP,  1,0,0},   // POP R1
        {Op::CMP,  0,1,0},   // CMP R0, R1
        {Op::HALT, 0,0,0},   // HALT
    };
}

// ── Örnek 2: Döngü (sayaç 0→9, düşük entropi = az çeşit) ────────────────────
static std::vector<Instr> makeLoop() {
    std::vector<Instr> v;
    // Başlatma
    v.push_back({Op::LOAD, 0,0,0});   // R0 = 0  (sayaç)
    v.push_back({Op::LOAD, 1,0,10});  // R1 = 10 (limit)
    v.push_back({Op::LOAD, 2,0,1});   // R2 = 1  (adım)

    // Döngü gövdesi (15 kez INC + CMP + JLT)
    for (int i = 0; i < 15; ++i) {
        v.push_back({Op::INC,  0,0,0});   // INC R0
        v.push_back({Op::CMP,  0,1,0});   // CMP R0, R1
        v.push_back({Op::ADD,  3,3,0});   // ADD R3, R3
        v.push_back({Op::JLT,  0,0,8});   // JLT R0, #8 (döngüye dön)
        v.push_back({Op::MOV,  4,0,0});   // MOV R4, R0
    }
    v.push_back({Op::OUT,  0,0,0});    // OUT R0
    v.push_back({Op::HALT, 0,0,0});    // HALT
    return v;
}

// ── Örnek 3: Yüksek entropili (rastgele talimatlar) ──────────────────────────
static std::vector<Instr> makePacked(int seed = 42) {
    const std::vector<uint8_t> pool = {
        Op::ADD,Op::SUB,Op::MUL,Op::DIV,Op::MOD,Op::INC,Op::DEC,
        Op::LOAD,Op::STORE,Op::MOV,Op::PUSH,Op::POP,
        Op::JMP,Op::JEQ,Op::JNE,Op::JLT,Op::JGT,Op::CALL,Op::RET,
        Op::AND,Op::OR,Op::XOR,Op::NOT,Op::CMP,
        Op::IN,Op::OUT
    };
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> opDist(0, (int)pool.size()-1);
    std::uniform_int_distribution<int> regDist(0, 7);
    std::uniform_int_distribution<int> immDist(0, 255);

    std::vector<Instr> v;
    v.reserve(200);
    for (int i = 0; i < 200; ++i) {
        v.push_back({
            pool[opDist(rng)],
            (uint8_t)regDist(rng),
            (uint8_t)regDist(rng),
            (uint8_t)immDist(rng)
        });
    }
    v.back() = {Op::HALT, 0, 0, 0};
    return v;
}

int main(int argc, char* argv[]) {
    std::string outDir = ".";
    if (argc > 1) outDir = argv[1];
    // Sondaki \ veya / kaldır
    while (!outDir.empty() && (outDir.back()=='/' || outDir.back()=='\\'))
        outDir.pop_back();

    std::cout << "opcgen — .opc örnek dosyaları üretiliyor...\n";
    std::cout << "Çıktı klasörü: " << outDir << "\n\n";

    bool ok = true;
    ok &= write(outDir + "/hello.opc",  makeHello());
    ok &= write(outDir + "/loop.opc",   makeLoop());
    ok &= write(outDir + "/packed.opc", makePacked());

    if (ok) {
        std::cout << "\nTamamlandı. Dosyaları OpcodeAnalyzer ile açabilirsiniz.\n";
        return 0;
    }
    return 1;
}
