#pragma once
#include <string>
#include <map>
#include <vector>
#include <ostream>

struct OpcodeStats {
    uint32_t count      = 0;
    double   percentage = 0.0;
};

struct OpcodeDiff {
    std::string mnemonic;
    uint32_t    count1        = 0;
    uint32_t    count2        = 0;
    int32_t     difference    = 0;
    double      changePercent = 0.0;
};

class Report {
public:
    std::string filename;
    uint32_t    totalInstructions = 0;
    uint32_t    uniqueOpcodes     = 0;
    double      entropy           = 0.0;
    std::string mostFrequent;

    std::map<std::string, OpcodeStats> opcodeFreq;    // mnemonic -> istatistik
    std::map<std::string, uint32_t>    categoryFreq;  // kategori  -> toplam

    // Karşılaştırma verisi (ComparisonAnalyzer tarafından doldurulur)
    bool                 isComparison = false;
    std::string          filename2;
    std::vector<OpcodeDiff> diffs;

    // Operatör aşırı yükleme: iki raporu birleştirir
    Report  operator+(const Report& other) const;
    Report& operator+=(const Report& other);

    // Stream çıkışı
    friend std::ostream& operator<<(std::ostream& os, const Report& r);

    // Dışa aktarma
    std::string toCSV()  const;
    std::string toHTML() const;

    bool isEmpty() const { return totalInstructions == 0; }
};
