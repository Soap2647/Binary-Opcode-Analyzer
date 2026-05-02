# 🛡️ Binary Opcode Analyzer (Binary Opcode Analiz Aracı)

![Version](https://img.shields.io/badge/version-2.0-blue.svg)
![C++](https://img.shields.io/badge/C++-17-00599C?logo=cplusplus)
![Qt](https://img.shields.io/badge/Qt-6.0+-41CD52?logo=qt)

**Binary Opcode Analyzer** is a powerful, modern, and professional tool designed to analyze the instruction sets of compiled binary files. Unlike basic analyzers, it supports real-world executables (EXE, DLL, SYS, BIN, ELF) and provides deep insights into x86 architecture distribution, entropy, and code structure.

**Binary Opcode Analiz Aracı**, derlenmiş binary dosyaların talimat setlerini analiz etmek için tasarlanmış güçlü ve modern bir araçtır. Basit analizörlerin aksine, gerçek dünyadaki çalıştırılabilir dosyaları (EXE, DLL, SYS, BIN, ELF) destekler ve x86 mimarisi dağılımı, entropi ve kod yapısı hakkında derinlemesine bilgi sağlar.

---

## 🚀 Key Features | Temel Özellikler

### 🔍 Deep Binary Analysis | Derin Binary Analizi
- **Multi-Format Support:** Analyze `.exe`, `.dll`, `.sys`, `.bin`, `.elf`, `.com`, and more.
- **Smart PE Parsing:** Automatically detects and focuses on the `.text` (code) section of Windows executables for accurate analysis.
- **x86 Opcode Coverage:** Comprehensive mapping of single-byte x86 instructions across 9 categories (Arithmetic, Logic, Data Transfer, Control Flow, etc.).
- **Entropy Analysis:** Calculates Shannon Entropy to detect packed or encrypted code segments.

### 🎨 Modern UI/UX | Modern Kullanıcı Deneyimi
- **Fluent Design:** A premium, state-of-the-art interface inspired by modern design systems.
- **Theme Support:** Fully integrated Dark and Light modes with high-contrast cyan accents.
- **Interactive Charts:** Visual distribution of opcode categories using interactive pie and bar charts.
- **Disassembly View:** A clean, per-byte disassembly table with categorical color coding.

### 📊 Reporting & Comparison | Raporlama ve Karşılaştırma
- **Export Options:** Export detailed analysis reports in professional **CSV** or **HTML** formats.
- **Side-by-Side Comparison:** Compare two different binary files to see differences in instruction distribution and code structure.

---

## 🛠️ Technology Stack | Teknolojik Yapı

- **Language:** C++17
- **Framework:** Qt 6 (Widgets, Charts)
- **Build System:** CMake
- **Design:** Custom QSS (Qt Style Sheets) with Fluent/Glassmorphism influence.

---

## 📥 Installation | Kurulum

### Prerequisites | Gereksinimler
- CMake 3.20+
- Qt 6.0+
- MSVC 2022 / GCC / Clang

### Build Instructions | Derleme Adımları
```bash
# Clone the repository | Depoyu kopyalayın
git clone https://github.com/yourusername/OpcodeAnalyzer.git
cd OpcodeAnalyzer

# Create build directory | Build dizini oluşturun
mkdir build
cd build

# Configure and Build | Yapılandırın ve Derleyin
cmake ..
cmake --build . --config Release
```

---

## 🤝 Contributing | Katkıda Bulunma
Contributions, issues, and feature requests are welcome!
Katkılarınız, sorun bildirimleriniz ve özellik talepleriniz bekliyoruz!

---

## 📄 License | Lisans
This project is licensed under the MIT License.
Bu proje MIT Lisansı ile lisanslanmıştır.

---
*Developed as an Academic Project for Object-Oriented Programming.*
*Nesneye Yönelik Programlama dersi kapsamında geliştirilmiştir.*
