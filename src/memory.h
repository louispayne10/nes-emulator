#pragma once

#include <array>
#include <cstdint>
#include <span>

class Memory
{
public:
    uint8_t read_byte(uint16_t addr) const;
    void write_byte(uint16_t addr, uint8_t data);
    uint16_t read_word(uint16_t addr) const;
    void write_word(uint16_t addr, uint16_t data);

    void write_rom(uint16_t start_addr, std::span<const uint8_t> buf);

    // private:
    uint8_t& access_byte(uint16_t addr);
    const uint8_t& access_byte(uint16_t addr) const;

    std::array<uint8_t, 0x800> m_InternalRam   = {};
    std::array<uint8_t, 0x08> m_PpuRegisters   = {};
    std::array<uint8_t, 0x18> m_ApuIoRegisters = {};
    std::array<uint8_t, 0x08> m_ApuIoExtended  = {};
    std::array<uint8_t, 0xBFE0> m_Rom          = {};
};
