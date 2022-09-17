#include "cpu.h"

#include "log.h"
#include "opcodes.h"
#include <utility>

static constexpr uint8_t BIT_7 = (1 << 7);

StatusRegFlag& operator|=(StatusRegFlag& a, StatusRegFlag b)
{
    a = (StatusRegFlag)((uint8_t)a | (uint8_t)b);
    return a;
}

bool operator&(StatusRegFlag a, StatusRegFlag b)
{
    return (uint8_t)a & (uint8_t)b;
}

bool operator==(StatusRegister lhs, StatusRegister rhs)
{
    return lhs.reg == rhs.reg;
}

CPU6502::CPU6502()
{
    m_InstructionMap[OPCODE_LDA_IMM]  = &CPU6502::lda_imm;
    m_InstructionMap[OPCODE_LDA_ZP]   = &CPU6502::lda_zp;
    m_InstructionMap[OPCODE_LDA_ZPX]  = &CPU6502::lda_zpx;
    m_InstructionMap[OPCODE_LDA_ABS]  = &CPU6502::lda_abs;
    m_InstructionMap[OPCODE_LDA_ABSX] = &CPU6502::lda_absx;
    m_InstructionMap[OPCODE_LDA_ABSY] = &CPU6502::lda_absy;
    m_InstructionMap[OPCODE_LDA_INDX] = &CPU6502::lda_indx;
    m_InstructionMap[OPCODE_LDA_INDY] = &CPU6502::lda_indy;

    m_InstructionMap[OPCODE_LDX_IMM]  = &CPU6502::ldx_imm;
    m_InstructionMap[OPCODE_LDX_ZP]   = &CPU6502::ldx_zp;
    m_InstructionMap[OPCODE_LDX_ZPY]  = &CPU6502::ldx_zpy;
    m_InstructionMap[OPCODE_LDX_ABS]  = &CPU6502::ldx_abs;
    m_InstructionMap[OPCODE_LDX_ABSY] = &CPU6502::ldx_absy;
}

void CPU6502::process_instruction()
{
    const uint8_t opcode  = memory.read_byte(registers.pc++);
    const auto handler_it = m_InstructionMap.find(opcode);
    if (handler_it == m_InstructionMap.end()) {
        NOT_IMPLEMENTED();
    }

    auto handler_fn = handler_it->second;
    (this->*handler_fn)();
}

uint8_t CPU6502::imm()
{
    return memory.read_byte(registers.pc++);
}

uint8_t CPU6502::zp()
{
    const uint8_t byte_addr = memory.read_byte(registers.pc++);
    return memory.read_byte(byte_addr);
}

uint8_t CPU6502::zpx()
{
    const uint8_t instruction_offset = memory.read_byte(registers.pc++);
    const uint8_t x                  = registers.x;

    // Let this addition wrap on overflow as the NES did
    const uint8_t byte_addr = instruction_offset + x;
    return memory.read_byte(byte_addr);
}

uint8_t CPU6502::zpy()
{
    const uint8_t operand = memory.read_byte(registers.pc++);
    const uint8_t y       = registers.y;

    // Let this addition wrap on overflow as the NES did
    const uint8_t byte_addr = operand + y;
    return memory.read_byte(byte_addr);
}

uint8_t CPU6502::abs()
{
    const uint16_t instruction_offset = memory.read_word(registers.pc);
    registers.pc += 2;
    return memory.read_byte(instruction_offset);
}

uint8_t CPU6502::absx()
{
    const uint16_t instruction_offset = memory.read_word(registers.pc);
    const uint8_t x                   = registers.x;

    registers.pc += 2;

    // Let it wrap on overflow - think that's correct behaviour
    const uint16_t byte_addr = instruction_offset + (uint16_t)x;
    return memory.read_byte(byte_addr);
}

uint8_t CPU6502::absy()
{
    const uint16_t instruction_offset = memory.read_word(registers.pc);
    const uint8_t y                   = registers.y;

    registers.pc += 2;

    // Let it wrap on overflow - think that's correct behaviour
    const uint16_t byte_addr = instruction_offset + (uint16_t)y;
    return memory.read_byte(byte_addr);
}

uint8_t CPU6502::indx()
{
    const uint8_t operand = memory.read_byte(registers.pc++);
    const uint8_t x       = registers.x;

    const uint8_t lsb_addr    = operand + x;
    const uint8_t lsb         = memory.read_byte(lsb_addr);
    const uint8_t msb         = memory.read_byte(lsb_addr + 1);
    const uint16_t final_addr = (uint16_t)((msb << 8) | lsb);
    return memory.read_byte(final_addr);
}

uint8_t CPU6502::indy()
{
    const uint8_t operand            = memory.read_byte(registers.pc++);
    const uint8_t lsb                = memory.read_byte(operand);
    const uint8_t msb                = memory.read_byte(operand + 1);
    const uint16_t intermediate_addr = (uint16_t)((msb << 8) | lsb);
    const uint16_t final_addr        = intermediate_addr + registers.y;
    return memory.read_byte(final_addr);
}

void CPU6502::lda_imm()
{
    lda(imm());
}

void CPU6502::lda_zp()
{
    lda(zp());
}

void CPU6502::lda_zpx()
{
    lda(zpx());
}

void CPU6502::lda_abs()
{
    lda(abs());
}

void CPU6502::lda_absx()
{
    lda(absx());
}

void CPU6502::lda_absy()
{
    lda(absy());
}

void CPU6502::lda_indx()
{
    lda(indx());
}

void CPU6502::lda_indy()
{
    lda(indy());
}

void CPU6502::ldx_imm()
{
    ldx(imm());
}

void CPU6502::ldx_zp()
{
    ldx(zp());
}

void CPU6502::ldx_zpy()
{
    ldx(zpy());
}

void CPU6502::ldx_abs()
{
    ldx(abs());
}

void CPU6502::ldx_absy()
{
    ldx(absy());
}

void CPU6502::lda(uint8_t data)
{
    if (data == 0) {
        registers.p.set_zero_flag();
    }
    if (data & BIT_7) {
        registers.p.set_negative_flag();
    }
    registers.a = data;
}

void CPU6502::ldx(uint8_t data)
{
    if (data == 0) {
        registers.p.set_zero_flag();
    }
    if (data & BIT_7) {
        registers.p.set_negative_flag();
    }
    registers.x = data;
}
