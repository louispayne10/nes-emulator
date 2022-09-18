#include "cpu.h"

#include "log.h"
#include "opcodes.h"
#include <utility>

static bool is_negative(uint8_t data)
{
    constexpr uint8_t BIT_7 = (1 << 7);
    return data & BIT_7;
}

static bool is_positive(uint8_t data)
{
    return !is_negative(data);
}

// takes a positive/negative number and converts it to the same magnitude but opposite sign in twos compliment
static uint8_t twos_compliment_flip(uint8_t data)
{
    return ~data + 1;
}

StatusRegFlag& operator|=(StatusRegFlag& a, StatusRegFlag b)
{
    a = (StatusRegFlag)((uint8_t)a | (uint8_t)b);
    return a;
}

StatusRegFlag& operator&=(StatusRegFlag& a, StatusRegFlag b)
{
    a = (StatusRegFlag)((uint8_t)a | (uint8_t)b);
    return a;
}

bool operator&(StatusRegFlag a, StatusRegFlag b)
{
    return (uint8_t)a & (uint8_t)b;
}

StatusRegFlag operator~(StatusRegFlag a)
{
    return (StatusRegFlag)(~(uint8_t)a);
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

    m_InstructionMap[OPCODE_AND_IMM]  = { "AND", &CPU6502::and_op, &CPU6502::imm };
    m_InstructionMap[OPCODE_AND_ZP]   = { "AND", &CPU6502::and_op, &CPU6502::zp };
    m_InstructionMap[OPCODE_AND_ZPX]  = { "AND", &CPU6502::and_op, &CPU6502::zpx };
    m_InstructionMap[OPCODE_AND_ABS]  = { "AND", &CPU6502::and_op, &CPU6502::abs };
    m_InstructionMap[OPCODE_AND_ABSX] = { "AND", &CPU6502::and_op, &CPU6502::absx };
    m_InstructionMap[OPCODE_AND_ABSY] = { "AND", &CPU6502::and_op, &CPU6502::absy };
    m_InstructionMap[OPCODE_AND_INDX] = { "AND", &CPU6502::and_op, &CPU6502::indx };
    m_InstructionMap[OPCODE_AND_INDY] = { "AND", &CPU6502::and_op, &CPU6502::indy };

    m_InstructionMap[OPCODE_ASL_ACC]  = { "ASL", &CPU6502::asl_acc, nullptr };
    m_InstructionMap[OPCODE_ASL_ZP]   = { "ASL", &CPU6502::asl, &CPU6502::zp };
    m_InstructionMap[OPCODE_ASL_ZPX]  = { "ASL", &CPU6502::asl, &CPU6502::zpx };
    m_InstructionMap[OPCODE_ASL_ABS]  = { "ASL", &CPU6502::asl, &CPU6502::abs };
    m_InstructionMap[OPCODE_ASL_ABSX] = { "ASL", &CPU6502::asl, &CPU6502::absx };

    m_InstructionMap[OPCODE_BCC_REL] = { "BCC", &CPU6502::bcc, &CPU6502::rel };

    m_InstructionMap[OPCODE_BCS_REL] = { "BCS", &CPU6502::bcs, &CPU6502::rel };

    m_InstructionMap[OPCODE_BEQ_REL] = { "BEQ", &CPU6502::beq, &CPU6502::rel };

    m_InstructionMap[OPCODE_BNE_REL] = { "BNE", &CPU6502::bne, &CPU6502::rel };
}

void CPU6502::process_instruction()
{
    const uint8_t opcode  = memory.read_byte(registers.pc++);
    const auto handler_it = m_InstructionMap.find(opcode);
    if (handler_it == m_InstructionMap.end()) {
        NOT_IMPLEMENTED();
    }

    auto op                  = handler_it->second.operation_fn;
    auto addr                = handler_it->second.addressing_fn;
    const uint16_t data_addr = addr ? (this->*addr)() : 0;
    (this->*op)(data_addr);
}

uint16_t CPU6502::imm()
{
    return registers.pc++;
}

uint16_t CPU6502::zp()
{
    return memory.read_byte(registers.pc++);
}

uint16_t CPU6502::zpx()
{
    const uint8_t instruction_offset = memory.read_byte(registers.pc++);
    const uint8_t x                  = registers.x;

    // Let this addition wrap on overflow as the NES did
    const uint8_t byte_addr = instruction_offset + x;
    return byte_addr;
}

uint16_t CPU6502::zpy()
{
    const uint8_t operand = memory.read_byte(registers.pc++);
    const uint8_t y       = registers.y;

    // Let this addition wrap on overflow as the NES did
    const uint8_t byte_addr = operand + y;
    return byte_addr;
}

uint16_t CPU6502::abs()
{
    const uint16_t instruction_offset = memory.read_word(registers.pc);
    registers.pc += 2;
    return instruction_offset;
}

uint16_t CPU6502::absx()
{
    const uint16_t instruction_offset = memory.read_word(registers.pc);
    const uint8_t x                   = registers.x;

    registers.pc += 2;

    // Let it wrap on overflow - think that's correct behaviour
    const uint16_t byte_addr = instruction_offset + (uint16_t)x;
    return byte_addr;
}

uint16_t CPU6502::absy()
{
    const uint16_t instruction_offset = memory.read_word(registers.pc);
    const uint8_t y                   = registers.y;

    registers.pc += 2;

    // Let it wrap on overflow - think that's correct behaviour
    const uint16_t byte_addr = instruction_offset + (uint16_t)y;
    return byte_addr;
}

uint16_t CPU6502::indx()
{
    const uint8_t operand = memory.read_byte(registers.pc++);
    const uint8_t x       = registers.x;

    const uint8_t lsb_addr    = operand + x;
    const uint8_t lsb         = memory.read_byte(lsb_addr);
    const uint8_t msb         = memory.read_byte(lsb_addr + 1);
    const uint16_t final_addr = (uint16_t)((msb << 8) | lsb);
    return final_addr;
}

uint16_t CPU6502::indy()
{
    const uint8_t operand            = memory.read_byte(registers.pc++);
    const uint8_t lsb                = memory.read_byte(operand);
    const uint8_t msb                = memory.read_byte(operand + 1);
    const uint16_t intermediate_addr = (uint16_t)((msb << 8) | lsb);
    const uint16_t final_addr        = intermediate_addr + registers.y;
    return final_addr;
}

uint16_t CPU6502::rel()
{
    return registers.pc++;
}

void CPU6502::lda(uint16_t data_addr)
{
    const uint8_t data = memory.read_byte(data_addr);
    if (data == 0) {
        registers.p.set_zero_flag();
    }
    if (is_negative(data)) {
        registers.p.set_negative_flag();
    }
    registers.a = data;
}

void CPU6502::ldx(uint16_t data_addr)
{
    const uint8_t data = memory.read_byte(data_addr);
    if (data == 0) {
        registers.p.set_zero_flag();
    }
    if (is_negative(data)) {
        registers.p.set_negative_flag();
    }
    registers.x = data;
}

void CPU6502::ldy(uint16_t data_addr)
{
    const uint8_t data = memory.read_byte(data_addr);
    if (data == 0) {
        registers.p.set_zero_flag();
    }
    if (is_negative(data)) {
        registers.p.set_negative_flag();
    }
    registers.y = data;
}

void CPU6502::adc(uint16_t data_addr)
{
    const uint8_t data = memory.read_byte(data_addr);
    const uint16_t res = registers.a + data + registers.p.carry_bit_set();
    if (res == 0) {
        registers.p.set_zero_flag();
    }
    if (is_negative(data)) {
        registers.p.set_negative_flag();
    }
    if (res & 0xFF00) {
        registers.p.set_carry_bit();
    }
    if (is_positive(registers.a) && is_positive(data) && is_negative(res)) {
        registers.p.set_overflow_bit();
    }
    if (is_negative(registers.a) && is_negative(data) && is_positive(res)) {
        registers.p.set_overflow_bit();
    }

    registers.a = res & 0xFF;
}

void CPU6502::and_op(uint16_t data_addr)
{
    const uint8_t data = memory.read_byte(data_addr);
    registers.a &= data;
    if (registers.a == 0) {
        registers.p.set_zero_flag();
    }
    if (is_negative(registers.a)) {
        registers.p.set_negative_flag();
    }
}

uint8_t CPU6502::asl_impl(uint8_t data)
{
    if (is_negative(data)) {
        registers.p.set_carry_bit();
    } else {
        registers.p.clear_carry_bit();
    }

    data <<= 1;
    if (data == 0) {
        registers.p.set_zero_flag();
    }
    if (is_negative(data)) {
        registers.p.set_negative_flag();
    }

    return data;
}

void CPU6502::asl(uint16_t data_addr)
{
    uint8_t data = memory.read_byte(data_addr);
    data         = asl_impl(data);
    memory.write_byte(data_addr, data);
}

void CPU6502::asl_acc(uint16_t data_addr)
{
    (void)data_addr; // this instruction operates directly on the accumulator
    registers.a = asl_impl(registers.a);
}

void CPU6502::displace_pc_from_data_addr(uint16_t data_addr)
{
    uint8_t data = memory.read_byte(data_addr);
    if (is_positive(data)) {
        registers.pc += data;
    } else {
        data = twos_compliment_flip(data);
        registers.pc -= data;
    }
}

void CPU6502::bcc(uint16_t data_addr)
{
    if (registers.p.carry_bit_set()) {
        return;
    }

    displace_pc_from_data_addr(data_addr);
}

void CPU6502::bcs(uint16_t data_addr)
{
    if (!registers.p.carry_bit_set()) {
        return;
    }

    displace_pc_from_data_addr(data_addr);
}

void CPU6502::beq(uint16_t data_addr)
{
    if (!registers.p.zero_flag_set()) {
        return;
    }

    displace_pc_from_data_addr(data_addr);
}

void CPU6502::bne(uint16_t data_addr)
{
    if (registers.p.zero_flag_set()) {
        return;
    }

    displace_pc_from_data_addr(data_addr);
}
