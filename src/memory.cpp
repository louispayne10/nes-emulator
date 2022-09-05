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

// uint16_t Memory::read_word(uint16_t addr) const { NOT_IMPLEMENTED(); }

// void Memory::write_word(uint16_t addr, uint16_t data) { NOT_IMPLEMENTED(); }
