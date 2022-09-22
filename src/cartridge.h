#pragma once

#include <cstdint>
#include <cstdio>
#include <memory>
#include <optional>
#include <span>

class Cartridge
{
public:
    static inline size_t max_size = 0x8000;
    static std::optional<Cartridge> from_file(const char* file_name);
    static std::optional<Cartridge> from_file(FILE* file);
    static std::optional<Cartridge> from_memory(std::span<uint8_t> mem);

    std::span<const uint8_t> get_program_data() const;
    std::span<const uint8_t> get_video_data() const;

private:
    struct [[gnu::packed]] Header
    {
        uint8_t sig[4];
        uint8_t prg_bank_count;
        uint8_t chr_bank_count;
        uint8_t control_byte_1;
        uint8_t control_byte_2;
        uint8_t ram_banks;
        uint8_t reserved[7];

        uint8_t get_mapper_number() const;
    };
    Header m_Header;

    std::unique_ptr<uint8_t[]> m_PrgData;
    std::unique_ptr<uint8_t[]> m_VideoData;
};
