#include "BinaryFile.h"
#include <fstream>
#include <stdexcept>
#include <cstring>

// Windows'ta std::ifstream ANSI code page kullanır; UTF-8 yollar için wstring gerekir.
#ifdef _WIN32
#include <windows.h>
static std::wstring toWide(const std::string& utf8) {
    if (utf8.empty()) return {};
    int len = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
    std::wstring w(len - 1, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, w.data(), len);
    return w;
}
#define OPEN_IFSTREAM(path, flags) std::ifstream(toWide(path), flags)
#else
#define OPEN_IFSTREAM(path, flags) std::ifstream(path, flags)
#endif

BinaryFile::BinaryFile(const std::string& path) {
    load(path);
}

void BinaryFile::load(const std::string& path) {
    m_path = path;

    auto file = OPEN_IFSTREAM(path, std::ios::binary | std::ios::ate);
    if (!file.is_open())
        throw std::runtime_error("Dosya açılamadı: " + path);

    m_fileSize = static_cast<uint64_t>(file.tellg());
    if (m_fileSize == 0)
        throw std::runtime_error("Dosya boş: " + path);

    file.seekg(0);

    // Tüm dosyayı oku
    std::vector<uint8_t> raw(m_fileSize);
    file.read(reinterpret_cast<char*>(raw.data()), static_cast<std::streamsize>(m_fileSize));

    // PE (.exe/.dll) tespiti
    if (tryParsePE(raw)) {
        m_isPE = true;
        // m_bytes ve m_baseOffset tryParsePE içinde dolduruldu
    } else {
        // Ham mod: tüm dosyayı analiz et
        m_isPE       = false;
        m_baseOffset = 0;
        m_bytes      = std::move(raw);
    }
}

// Basit PE parser: IMAGE_DOS_HEADER -> IMAGE_NT_HEADERS -> .text section
bool BinaryFile::tryParsePE(const std::vector<uint8_t>& raw) {
    // MZ imzası
    if (raw.size() < 0x40) return false;
    if (raw[0] != 'M' || raw[1] != 'Z') return false;

    // e_lfanew (offset 0x3C, 4 byte)
    uint32_t e_lfanew = 0;
    std::memcpy(&e_lfanew, raw.data() + 0x3C, 4);
    if (e_lfanew + 4 > raw.size()) return false;

    // PE imzası
    if (raw[e_lfanew]     != 'P' || raw[e_lfanew + 1] != 'E' ||
        raw[e_lfanew + 2] != 0   || raw[e_lfanew + 3] != 0)
        return false;

    // COFF header: offset e_lfanew + 4
    uint32_t coffOff = e_lfanew + 4;
    if (coffOff + 20 > raw.size()) return false;

    uint16_t numSections   = 0;
    uint16_t optHeaderSize = 0;
    std::memcpy(&numSections,   raw.data() + coffOff + 2, 2);
    std::memcpy(&optHeaderSize, raw.data() + coffOff + 16, 2);

    // Section table başlangıcı: coffOff + 20 (COFF) + optHeaderSize
    uint32_t sectionTableOff = coffOff + 20 + optHeaderSize;

    // Her section 40 byte
    for (uint16_t i = 0; i < numSections; ++i) {
        uint32_t sOff = sectionTableOff + i * 40;
        if (sOff + 40 > raw.size()) break;

        // Section name: 8 bytes
        char name[9] = {};
        std::memcpy(name, raw.data() + sOff, 8);

        // .text section'ı bul (isim ".text\0" ya da ".text" ile başlar)
        if (std::strncmp(name, ".text", 5) == 0) {
            uint32_t rawDataSize   = 0;
            uint32_t rawDataOffset = 0;
            std::memcpy(&rawDataSize,   raw.data() + sOff + 16, 4);
            std::memcpy(&rawDataOffset, raw.data() + sOff + 20, 4);

            if (rawDataOffset == 0 || rawDataOffset >= raw.size()) return false;
            if (rawDataOffset + rawDataSize > raw.size())
                rawDataSize = static_cast<uint32_t>(raw.size() - rawDataOffset);

            m_baseOffset = rawDataOffset;
            m_bytes.assign(raw.begin() + rawDataOffset,
                           raw.begin() + rawDataOffset + rawDataSize);
            return true;
        }
    }

    // .text bulunamadı — tüm dosyayı ham al ama PE olarak işaretle
    m_baseOffset = 0;
    m_bytes = raw;
    return true;
}
