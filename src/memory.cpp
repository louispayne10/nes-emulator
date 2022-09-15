#include "memory.h"

#include "log.h"


uint8_t Memory::read_byte(uint16_t addr) const
{
    return m_Memory[addr];
}

void Memory::write_byte(uint16_t addr, uint8_t data)
{
    m_Memory[addr] = data;
}

uint16_t Memory::read_word(uint16_t addr) const
{
    const uint8_t hi_byte = m_Memory[addr];
    const uint8_t lo_byte = m_Memory[addr + 1];
    return (uint16_t)((hi_byte << 8U) | lo_byte);
}

void Memory::write_word(uint16_t addr, uint16_t data)
{
    const uint8_t hi_byte = (uint8_t)((data & 0xFF00U) >> 8U);
    const uint8_t lo_byte = data & 0xFFU;
    m_Memory[addr]        = hi_byte;
    m_Memory[addr + 1]    = lo_byte;
}
