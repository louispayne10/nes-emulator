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
    m_InstructionMap[OPCODE_LDA_IMM]  = { "LDA", &CPU6502::lda, &CPU6502::imm };
    m_InstructionMap[OPCODE_LDA_ZP]   = { "LDA", &CPU6502::lda, &CPU6502::zp };
    m_InstructionMap[OPCODE_LDA_ZPX]  = { "LDA", &CPU6502::lda, &CPU6502::zpx };
    m_InstructionMap[OPCODE_LDA_ABS]  = { "LDA", &CPU6502::lda, &CPU6502::abs };
    m_InstructionMap[OPCODE_LDA_ABSX] = { "LDA", &CPU6502::lda, &CPU6502::absx };
    m_InstructionMap[OPCODE_LDA_ABSY] = { "LDA", &CPU6502::lda, &CPU6502::absy };
    m_InstructionMap[OPCODE_LDA_INDX] = { "LDA", &CPU6502::lda, &CPU6502::indx };
    m_InstructionMap[OPCODE_LDA_INDY] = { "LDA", &CPU6502::lda, &CPU6502::indy };

    m_InstructionMap[OPCODE_LDX_IMM]  = { "LDX", &CPU6502::ldx, &CPU6502::imm };
    m_InstructionMap[OPCODE_LDX_ZP]   = { "LDX", &CPU6502::ldx, &CPU6502::zp };
    m_InstructionMap[OPCODE_LDX_ZPY]  = { "LDX", &CPU6502::ldx, &CPU6502::zpy };
    m_InstructionMap[OPCODE_LDX_ABS]  = { "LDX", &CPU6502::ldx, &CPU6502::abs };
    m_InstructionMap[OPCODE_LDX_ABSY] = { "LDX", &CPU6502::ldx, &CPU6502::absy };

    m_InstructionMap[OPCODE_LDY_IMM]  = { "LDY", &CPU6502::ldy, &CPU6502::imm };
    m_InstructionMap[OPCODE_LDY_ZP]   = { "LDY", &CPU6502::ldy, &CPU6502::zp };
    m_InstructionMap[OPCODE_LDY_ZPX]  = { "LDY", &CPU6502::ldy, &CPU6502::zpx };
    m_InstructionMap[OPCODE_LDY_ABS]  = { "LDY", &CPU6502::ldy, &CPU6502::abs };
    m_InstructionMap[OPCODE_LDY_ABSX] = { "LDY", &CPU6502::ldy, &CPU6502::absx };

    m_InstructionMap[OPCODE_ADC_IMM]  = { "ADC", &CPU6502::adc, &CPU6502::imm };
    m_InstructionMap[OPCODE_ADC_ZP]   = { "ADC", &CPU6502::adc, &CPU6502::zp };
    m_InstructionMap[OPCODE_ADC_ZPX]  = { "ADC", &CPU6502::adc, &CPU6502::zpx };
    m_InstructionMap[OPCODE_ADC_ABS]  = { "ADC", &CPU6502::adc, &CPU6502::abs };
    m_InstructionMap[OPCODE_ADC_ABSX] = { "ADC", &CPU6502::adc, &CPU6502::absx };
    m_InstructionMap[OPCODE_ADC_ABSY] = { "ADC", &CPU6502::adc, &CPU6502::absy };
    m_InstructionMap[OPCODE_ADC_INDX] = { "ADC", &CPU6502::adc, &CPU6502::indx };
    m_InstructionMap[OPCODE_ADC_INDY] = { "ADC", &CPU6502::adc, &CPU6502::indy };
}

void CPU6502::process_instruction()
{
    const uint8_t opcode  = memory.read_byte(registers.pc++);
    const auto handler_it = m_InstructionMap.find(opcode);
    if (handler_it == m_InstructionMap.end()) {
        NOT_IMPLEMENTED();
    }

    auto op            = handler_it->second.operation_fn;
    auto addr          = handler_it->second.addressing_fn;
    const uint8_t data = (this->*addr)();
    (this->*op)(data);
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

void CPU6502::ldy(uint8_t data)
{
    if (data == 0) {
        registers.p.set_zero_flag();
    }
    if (data & BIT_7) {
        registers.p.set_negative_flag();
    }
    registers.y = data;
}

void CPU6502::adc(uint8_t data)
{
    const uint16_t res = registers.a + data + registers.p.carry_bit_set();
    if (res == 0) {
        registers.p.set_zero_flag();
    }
    if (res & BIT_7) {
        registers.p.set_negative_flag();
    }
    if (res & 0xFF00) {
        registers.p.set_carry_bit();
    }
    if (!(registers.a & BIT_7) && !(data & BIT_7) && (res & BIT_7)) {
        // In this case two positive numbers added to a negative number
        registers.p.set_overflow_bit();
    }
    if ((registers.a & BIT_7) && (data & BIT_7) && !(res & BIT_7)) {
        // In this case two negative numbers added to a positive number
        registers.p.set_overflow_bit();
    }

    registers.a = res & 0xFF;
}
