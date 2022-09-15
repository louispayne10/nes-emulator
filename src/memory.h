#pragma once

#include <array>
#include <cstdint>

class Memory
{
public:
    uint8_t read_byte(uint16_t addr) const;
    void write_byte(uint16_t addr, uint8_t data);
    uint16_t read_word(uint16_t addr) const;
    void write_word(uint16_t addr, uint16_t data);

private:
    std::array<uint8_t, 0xFFFF> m_Memory = {};
};
