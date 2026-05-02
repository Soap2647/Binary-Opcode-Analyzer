#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <stdexcept>

// Herhangi bir binary dosyayı ham byte dizisi olarak okur.
// Windows PE (.exe/.dll) ise .text section'ını bulmaya çalışır;
// bulamazsa tüm dosyayı ham analiz eder.
class BinaryFile {
public:
    explicit BinaryFile(const std::string& path);

    const std::vector<uint8_t>& bytes()    const { return m_bytes;    }
    const std::string&          path()     const { return m_path;     }
    uint64_t                    fileSize() const { return m_fileSize; }
    bool                        isPE()     const { return m_isPE;     }
    uint64_t                    baseOffset() const { return m_baseOffset; }

private:
    void load(const std::string& path);
    bool tryParsePE(const std::vector<uint8_t>& raw);

    std::string          m_path;
    uint64_t             m_fileSize   = 0;
    bool                 m_isPE       = false;
    uint64_t             m_baseOffset = 0;   // .text section file offset (or 0)
    std::vector<uint8_t> m_bytes;            // analysable bytes
};
