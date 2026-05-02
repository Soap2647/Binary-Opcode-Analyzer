#include "Report.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctime>

Report Report::operator+(const Report& other) const {
    Report result;
    result.filename          = filename + " + " + other.filename;
    result.totalInstructions = totalInstructions + other.totalInstructions;
    result.entropy           = (entropy + other.entropy) / 2.0;

    // Opcode frekanslarını birleştir
    for (const auto& [mnem, stats] : opcodeFreq)
        result.opcodeFreq[mnem].count += stats.count;
    for (const auto& [mnem, stats] : other.opcodeFreq)
        result.opcodeFreq[mnem].count += stats.count;

    // Yüzdeleri yeniden hesapla
    if (result.totalInstructions > 0) {
        for (auto& [mnem, stats] : result.opcodeFreq) {
            stats.percentage = 100.0 * stats.count / result.totalInstructions;
        }
    }

    // Kategori frekanslarını birleştir
    for (const auto& [cat, cnt] : categoryFreq)
        result.categoryFreq[cat] += cnt;
    for (const auto& [cat, cnt] : other.categoryFreq)
        result.categoryFreq[cat] += cnt;

    result.uniqueOpcodes = static_cast<uint32_t>(result.opcodeFreq.size());

    if (!result.opcodeFreq.empty()) {
        auto it = std::max_element(result.opcodeFreq.begin(), result.opcodeFreq.end(),
            [](const auto& a, const auto& b){ return a.second.count < b.second.count; });
        result.mostFrequent = it->first;
    }

    return result;
}

Report& Report::operator+=(const Report& other) {
    *this = *this + other;
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Report& r) {
    os << "=== Rapor: " << r.filename << " ===\n"
       << "Toplam talimat : " << r.totalInstructions << "\n"
       << "Benzersiz opcode: " << r.uniqueOpcodes << "\n"
       << "Entropi        : " << std::fixed << std::setprecision(4) << r.entropy << "\n"
       << "En sık         : " << r.mostFrequent << "\n\n"
       << std::left << std::setw(8) << "Opcode"
       << std::setw(8) << "Sayı"
       << "Yüzde\n"
       << std::string(30, '-') << "\n";
    for (const auto& [mnem, stats] : r.opcodeFreq) {
        os << std::setw(8) << mnem
           << std::setw(8) << stats.count
           << std::fixed << std::setprecision(1) << stats.percentage << "%\n";
    }
    return os;
}

std::string Report::toCSV() const {
    std::ostringstream ss;
    ss << "Opcode,Sayı,Yüzde,Kategori\n";
    for (const auto& [mnem, stats] : opcodeFreq) {
        ss << mnem << "," << stats.count << ","
           << std::fixed << std::setprecision(2) << stats.percentage << "\n";
    }
    return ss.str();
}

std::string Report::toHTML() const {
    std::time_t t = std::time(nullptr);
    char timeBuf[64];
    std::strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));

    std::ostringstream ss;
    ss << R"(<!DOCTYPE html>
<html lang="tr">
<head>
<meta charset="UTF-8">
<title>Opcode Analiz Raporu</title>
<style>
  body{font-family:'Segoe UI',sans-serif;background:#1e1e2e;color:#cdd6f4;margin:40px}
  h1{color:#89b4fa}h2{color:#89b4fa;border-bottom:1px solid #313244;padding-bottom:6px}
  .kpi{display:flex;gap:20px;margin:20px 0}
  .card{background:#181825;border:1px solid #313244;border-radius:10px;padding:20px 30px;text-align:center}
  .card .val{font-size:2em;font-weight:bold;color:#89b4fa}
  .card .lbl{color:#a6adc8;font-size:.85em;margin-top:4px}
  table{border-collapse:collapse;width:100%;margin-top:10px}
  th{background:#181825;color:#89b4fa;padding:10px 14px;text-align:left;border-bottom:2px solid #89b4fa}
  td{padding:8px 14px;border-bottom:1px solid #313244}
  tr:hover td{background:#313244}
  .bar-wrap{background:#313244;border-radius:4px;height:16px;min-width:200px}
  .bar{background:#89b4fa;border-radius:4px;height:100%}
  footer{margin-top:40px;color:#6c6f85;font-size:.8em}
</style>
</head>
<body>
<h1>Opcode Analiz Raporu</h1>
<p>Dosya: <strong>)" << filename << R"(</strong> &nbsp;|&nbsp; Oluşturma: )" << timeBuf << R"(</p>
<div class="kpi">
  <div class="card"><div class="val">)" << totalInstructions << R"(</div><div class="lbl">Toplam Talimat</div></div>
  <div class="card"><div class="val">)" << uniqueOpcodes << R"(</div><div class="lbl">Benzersiz Opcode</div></div>
  <div class="card"><div class="val">)" << std::fixed << std::setprecision(2) << entropy
      << R"(</div><div class="lbl">Shannon Entropi</div></div>
  <div class="card"><div class="val">)" << mostFrequent << R"(</div><div class="lbl">En Sık Opcode</div></div>
</div>
<h2>Opcode Frekans Tablosu</h2>
<table>
<tr><th>Opcode</th><th>Sayı</th><th>Yüzde</th><th>Dağılım</th></tr>
)";
    for (const auto& [mnem, stats] : opcodeFreq) {
        int barW = static_cast<int>(stats.percentage * 2); // maks 200px
        ss << "<tr><td>" << mnem << "</td><td>" << stats.count << "</td><td>"
           << std::fixed << std::setprecision(1) << stats.percentage
           << "%</td><td><div class='bar-wrap'><div class='bar' style='width:"
           << barW << "px'></div></div></td></tr>\n";
    }
    ss << R"(</table>
<h2>Kategori Dağılımı</h2>
<table>
<tr><th>Kategori</th><th>Sayı</th></tr>
)";
    for (const auto& [cat, cnt] : categoryFreq)
        ss << "<tr><td>" << cat << "</td><td>" << cnt << "</td></tr>\n";

    ss << R"(</table>
<footer>OpcodeAnalyzer tarafından oluşturuldu</footer>
</body></html>)";
    return ss.str();
}
