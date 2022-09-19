#include "memory.h"

#include "log.h"

uint8_t& Memory::access_byte(uint16_t addr)
{
    if (addr < 0x2000) {
        return m_InternalRam[addr % 0x800];
    } else if (addr < 0x4008) {
        return m_PpuRegisters[(addr - 0x2000) % 0x08];
    } else if (addr < 0x4018) {
        return m_ApuIoRegisters[addr - 0x4000];
    } else if (addr < 0x4020) {
        return m_ApuIoExtended[addr - 0x4018];
    }

    return m_Cartridge[addr - 0x4020];
}

const uint8_t& Memory::access_byte(uint16_t addr) const
{
    return const_cast<Memory*>(this)->access_byte(addr);
}

uint8_t Memory::read_byte(uint16_t addr) const
{
    return access_byte(addr);
}

void Memory::write_byte(uint16_t addr, uint8_t data)
{
    access_byte(addr) = data;
}

uint16_t Memory::read_word(uint16_t addr) const
{
    const uint8_t lo_byte = access_byte(addr);
    const uint8_t hi_byte = access_byte(addr + 1);
    return (uint16_t)((hi_byte << 8) | lo_byte);
}

void Memory::write_word(uint16_t addr, uint16_t data)
{
    const uint8_t hi_byte = (uint8_t)((data & 0xFF00) >> 8);
    const uint8_t lo_byte = data & 0xFFU;
    access_byte(addr)     = lo_byte;
    access_byte(addr + 1) = hi_byte;
}
