#include "cpu.h"

#include "log.h"
#include "opcodes.h"
#include <cstdint>
#include <utility>

static constexpr uint8_t BIT_6 = (1 << 6);
static constexpr uint8_t BIT_7 = (1 << 7);

static bool is_negative(uint8_t data)
{
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
    a = (StatusRegFlag)((uint8_t)a & (uint8_t)b);
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
    m_InstructionMap[OPCODE_LDA_IMM]  = { "LDA", &CPU6502::lda, &CPU6502::imm, 2 };
    m_InstructionMap[OPCODE_LDA_ZP]   = { "LDA", &CPU6502::lda, &CPU6502::zp, 3 };
    m_InstructionMap[OPCODE_LDA_ZPX]  = { "LDA", &CPU6502::lda, &CPU6502::zpx, 4 };
    m_InstructionMap[OPCODE_LDA_ABS]  = { "LDA", &CPU6502::lda, &CPU6502::abs, 4 };
    m_InstructionMap[OPCODE_LDA_ABSX] = { "LDA", &CPU6502::lda, &CPU6502::absx, 4 };
    m_InstructionMap[OPCODE_LDA_ABSY] = { "LDA", &CPU6502::lda, &CPU6502::absy, 4 };
    m_InstructionMap[OPCODE_LDA_INDX] = { "LDA", &CPU6502::lda, &CPU6502::indx, 6 };
    m_InstructionMap[OPCODE_LDA_INDY] = { "LDA", &CPU6502::lda, &CPU6502::indy, 5 };

    m_InstructionMap[OPCODE_LDX_IMM]  = { "LDX", &CPU6502::ldx, &CPU6502::imm, 2 };
    m_InstructionMap[OPCODE_LDX_ZP]   = { "LDX", &CPU6502::ldx, &CPU6502::zp, 3 };
    m_InstructionMap[OPCODE_LDX_ZPY]  = { "LDX", &CPU6502::ldx, &CPU6502::zpy, 4 };
    m_InstructionMap[OPCODE_LDX_ABS]  = { "LDX", &CPU6502::ldx, &CPU6502::abs, 4 };
    m_InstructionMap[OPCODE_LDX_ABSY] = { "LDX", &CPU6502::ldx, &CPU6502::absy, 4 };

    m_InstructionMap[OPCODE_LDY_IMM]  = { "LDY", &CPU6502::ldy, &CPU6502::imm, 2 };
    m_InstructionMap[OPCODE_LDY_ZP]   = { "LDY", &CPU6502::ldy, &CPU6502::zp, 3 };
    m_InstructionMap[OPCODE_LDY_ZPX]  = { "LDY", &CPU6502::ldy, &CPU6502::zpx, 4 };
    m_InstructionMap[OPCODE_LDY_ABS]  = { "LDY", &CPU6502::ldy, &CPU6502::abs, 4 };
    m_InstructionMap[OPCODE_LDY_ABSX] = { "LDY", &CPU6502::ldy, &CPU6502::absx, 4 };

    m_InstructionMap[OPCODE_ADC_IMM]  = { "ADC", &CPU6502::adc, &CPU6502::imm, 2 };
    m_InstructionMap[OPCODE_ADC_ZP]   = { "ADC", &CPU6502::adc, &CPU6502::zp, 3 };
    m_InstructionMap[OPCODE_ADC_ZPX]  = { "ADC", &CPU6502::adc, &CPU6502::zpx, 4 };
    m_InstructionMap[OPCODE_ADC_ABS]  = { "ADC", &CPU6502::adc, &CPU6502::abs, 4 };
    m_InstructionMap[OPCODE_ADC_ABSX] = { "ADC", &CPU6502::adc, &CPU6502::absx, 4 };
    m_InstructionMap[OPCODE_ADC_ABSY] = { "ADC", &CPU6502::adc, &CPU6502::absy, 4 };
    m_InstructionMap[OPCODE_ADC_INDX] = { "ADC", &CPU6502::adc, &CPU6502::indx, 6 };
    m_InstructionMap[OPCODE_ADC_INDY] = { "ADC", &CPU6502::adc, &CPU6502::indy, 5 };

    m_InstructionMap[OPCODE_AND_IMM]  = { "AND", &CPU6502::and_op, &CPU6502::imm, 2 };
    m_InstructionMap[OPCODE_AND_ZP]   = { "AND", &CPU6502::and_op, &CPU6502::zp, 3 };
    m_InstructionMap[OPCODE_AND_ZPX]  = { "AND", &CPU6502::and_op, &CPU6502::zpx, 4 };
    m_InstructionMap[OPCODE_AND_ABS]  = { "AND", &CPU6502::and_op, &CPU6502::abs, 4 };
    m_InstructionMap[OPCODE_AND_ABSX] = { "AND", &CPU6502::and_op, &CPU6502::absx, 4 };
    m_InstructionMap[OPCODE_AND_ABSY] = { "AND", &CPU6502::and_op, &CPU6502::absy, 4 };
    m_InstructionMap[OPCODE_AND_INDX] = { "AND", &CPU6502::and_op, &CPU6502::indx, 6 };
    m_InstructionMap[OPCODE_AND_INDY] = { "AND", &CPU6502::and_op, &CPU6502::indy, 5 };

    m_InstructionMap[OPCODE_ASL_ACC]  = { "ASL", &CPU6502::asl_acc, nullptr, 2 };
    m_InstructionMap[OPCODE_ASL_ZP]   = { "ASL", &CPU6502::asl, &CPU6502::zp, 5 };
    m_InstructionMap[OPCODE_ASL_ZPX]  = { "ASL", &CPU6502::asl, &CPU6502::zpx, 6 };
    m_InstructionMap[OPCODE_ASL_ABS]  = { "ASL", &CPU6502::asl, &CPU6502::abs, 6 };
    m_InstructionMap[OPCODE_ASL_ABSX] = { "ASL", &CPU6502::asl, &CPU6502::absx, 7 };

    m_InstructionMap[OPCODE_BCC_REL] = { "BCC", &CPU6502::bcc, &CPU6502::rel, 2 };

    m_InstructionMap[OPCODE_BCS_REL] = { "BCS", &CPU6502::bcs, &CPU6502::rel, 2 };

    m_InstructionMap[OPCODE_BEQ_REL] = { "BEQ", &CPU6502::beq, &CPU6502::rel, 2 };

    m_InstructionMap[OPCODE_BNE_REL] = { "BNE", &CPU6502::bne, &CPU6502::rel, 2 };

    m_InstructionMap[OPCODE_BIT_ZP]  = { "BIT", &CPU6502::bit, &CPU6502::zp, 2 };
    m_InstructionMap[OPCODE_BIT_ABS] = { "BIT", &CPU6502::bit, &CPU6502::abs, 3 };

    m_InstructionMap[OPCODE_BMI_REL] = { "BMI", &CPU6502::bmi, &CPU6502::rel, 2 };

    m_InstructionMap[OPCODE_BPL_REL] = { "BPL", &CPU6502::bpl, &CPU6502::rel, 2 };

    m_InstructionMap[OPCODE_BRK_IMP] = { "BRK", &CPU6502::brk, &CPU6502::imp, 7 };

    m_InstructionMap[OPCODE_BVC_REL] = { "BCV", &CPU6502::bvc, &CPU6502::rel, 2 };

    m_InstructionMap[OPCODE_BVS_REL] = { "BVS", &CPU6502::bvs, &CPU6502::rel, 2 };

    m_InstructionMap[OPCODE_CLC_IMP] = { "CLC", &CPU6502::clc, &CPU6502::imp, 2 };
    m_InstructionMap[OPCODE_CLD_IMP] = { "CLD", &CPU6502::cld, &CPU6502::imp, 2 };
    m_InstructionMap[OPCODE_CLI_IMP] = { "CLI", &CPU6502::cli, &CPU6502::imp, 2 };
    m_InstructionMap[OPCODE_CLV_IMP] = { "CLV", &CPU6502::clv, &CPU6502::imp, 2 };

    m_InstructionMap[OPCODE_CMP_IMM]  = { "CMP", &CPU6502::cmp, &CPU6502::imm, 2 };
    m_InstructionMap[OPCODE_CMP_ZP]   = { "CMP", &CPU6502::cmp, &CPU6502::zp, 3 };
    m_InstructionMap[OPCODE_CMP_ZPX]  = { "CMP", &CPU6502::cmp, &CPU6502::zpx, 4 };
    m_InstructionMap[OPCODE_CMP_ABS]  = { "CMP", &CPU6502::cmp, &CPU6502::abs, 4 };
    m_InstructionMap[OPCODE_CMP_ABSX] = { "CMP", &CPU6502::cmp, &CPU6502::absx, 4 };
    m_InstructionMap[OPCODE_CMP_ABSY] = { "CMP", &CPU6502::cmp, &CPU6502::absy, 4 };
    m_InstructionMap[OPCODE_CMP_INDX] = { "CMP", &CPU6502::cmp, &CPU6502::indx, 6 };
    m_InstructionMap[OPCODE_CMP_INDY] = { "CMP", &CPU6502::cmp, &CPU6502::indy, 5 };

    m_InstructionMap[OPCODE_CPX_IMM] = { "CPX", &CPU6502::cpx, &CPU6502::imm, 2 };
    m_InstructionMap[OPCODE_CPX_ZP]  = { "CPX", &CPU6502::cpx, &CPU6502::zp, 3 };
    m_InstructionMap[OPCODE_CPX_ABS] = { "CPX", &CPU6502::cpx, &CPU6502::abs, 4 };

    m_InstructionMap[OPCODE_CPY_IMM] = { "CPY", &CPU6502::cpy, &CPU6502::imm, 2 };
    m_InstructionMap[OPCODE_CPY_ZP]  = { "CPY", &CPU6502::cpy, &CPU6502::zp, 3 };
    m_InstructionMap[OPCODE_CPY_ABS] = { "CPY", &CPU6502::cpy, &CPU6502::abs, 4 };

    m_InstructionMap[OPCODE_DEC_ZP]   = { "DEC", &CPU6502::dec, &CPU6502::zp, 5 };
    m_InstructionMap[OPCODE_DEC_ZPX]  = { "DEC", &CPU6502::dec, &CPU6502::zpx, 6 };
    m_InstructionMap[OPCODE_DEC_ABS]  = { "DEC", &CPU6502::dec, &CPU6502::abs, 6 };
    m_InstructionMap[OPCODE_DEC_ABSX] = { "DEC", &CPU6502::dec, &CPU6502::absx, 7 };

    m_InstructionMap[OPCODE_DEX_IMP] = { "DEX", &CPU6502::dex, &CPU6502::imp, 2 };
    m_InstructionMap[OPCODE_DEY_IMP] = { "DEY", &CPU6502::dey, &CPU6502::imp, 2 };

    m_InstructionMap[OPCODE_EOR_IMM]  = { "EOR", &CPU6502::eor, &CPU6502::imm, 2 };
    m_InstructionMap[OPCODE_EOR_ZP]   = { "EOR", &CPU6502::eor, &CPU6502::zp, 3 };
    m_InstructionMap[OPCODE_EOR_ZPX]  = { "EOR", &CPU6502::eor, &CPU6502::zpx, 4 };
    m_InstructionMap[OPCODE_EOR_ABS]  = { "EOR", &CPU6502::eor, &CPU6502::abs, 4 };
    m_InstructionMap[OPCODE_EOR_ABSX] = { "EOR", &CPU6502::eor, &CPU6502::indx, 4 };
    m_InstructionMap[OPCODE_EOR_ABSY] = { "EOR", &CPU6502::eor, &CPU6502::absy, 4 };
    m_InstructionMap[OPCODE_EOR_INDX] = { "EOR", &CPU6502::eor, &CPU6502::indx, 6 };
    m_InstructionMap[OPCODE_EOR_INDY] = { "EOR", &CPU6502::eor, &CPU6502::indy, 5 };

    m_InstructionMap[OPCODE_INC_ZP]   = { "INC", &CPU6502::inc, &CPU6502::zp, 5 };
    m_InstructionMap[OPCODE_INC_ZPX]  = { "INC", &CPU6502::inc, &CPU6502::zpx, 6 };
    m_InstructionMap[OPCODE_INC_ABS]  = { "INC", &CPU6502::inc, &CPU6502::abs, 6 };
    m_InstructionMap[OPCODE_INC_ABSX] = { "INC", &CPU6502::inc, &CPU6502::absx, 7 };
}

void CPU6502::next_cycle()
{
    if (cycles_remaining > 0) {
        cycles_remaining--;
        return;
    }

    cycles_remaining = process_instruction() - 1;
}

uint8_t CPU6502::process_instruction()
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
    return handler_it->second.cycles;
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

uint16_t CPU6502::imp()
{
    return 0;
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
        registers.p.clear_carry_flag();
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
    if (!registers.p.carry_bit_set()) {
        displace_pc_from_data_addr(data_addr);
    }
}

void CPU6502::bcs(uint16_t data_addr)
{
    if (registers.p.carry_bit_set()) {
        displace_pc_from_data_addr(data_addr);
        return;
    }
}

void CPU6502::beq(uint16_t data_addr)
{
    if (registers.p.zero_flag_set()) {
        displace_pc_from_data_addr(data_addr);
    }
}

void CPU6502::bne(uint16_t data_addr)
{
    if (!registers.p.zero_flag_set()) {
        displace_pc_from_data_addr(data_addr);
    }
}

void CPU6502::bit(uint16_t data_addr)
{
    const uint8_t data   = memory.read_byte(data_addr);
    const uint8_t result = data & registers.a;
    if (result) {
        registers.p.clear_zero_flag();
    } else {
        registers.p.set_zero_flag();
    }

    if (data & BIT_7)
        registers.p.set_negative_flag();
    else
        registers.p.clear_negative_flag();
    if (data & BIT_6)
        registers.p.set_overflow_bit();
    else
        registers.p.clear_overflow_flag();
}

void CPU6502::bmi(uint16_t data_addr)
{
    if (registers.p.negative_flag_set()) {
        displace_pc_from_data_addr(data_addr);
    }
}

void CPU6502::bpl(uint16_t data_addr)
{
    if (!registers.p.negative_flag_set()) {
        displace_pc_from_data_addr(data_addr);
    }
}

void CPU6502::brk(uint16_t data_addr)
{
    (void)data_addr;
    NOT_IMPLEMENTED();
}

void CPU6502::bvc(uint16_t data_addr)
{
    if (!registers.p.overflow_flag_set()) {
        displace_pc_from_data_addr(data_addr);
    }
}

void CPU6502::bvs(uint16_t data_addr)
{
    if (registers.p.overflow_flag_set()) {
        displace_pc_from_data_addr(data_addr);
    }
}

void CPU6502::clc(uint16_t data_addr)
{
    (void)data_addr;
    registers.p.clear_carry_flag();
}

void CPU6502::cld(uint16_t data_addr)
{
    (void)data_addr;
    NOT_IMPLEMENTED();
}

void CPU6502::cli(uint16_t data_addr)
{
    (void)data_addr;
    registers.p.clear_int_disable_flag();
}

void CPU6502::clv(uint16_t data_addr)
{
    (void)data_addr;
    registers.p.clear_overflow_flag();
}

void CPU6502::cmp(uint16_t data_addr)
{
    const uint8_t data = memory.read_byte(data_addr);
    if (registers.a >= data) {
        registers.p.set_carry_bit();
    } else {
        registers.p.clear_carry_flag();
    }
    if (registers.a == data) {
        registers.p.set_zero_flag();
    } else {
        registers.p.clear_zero_flag();
    }

    // TODO: validate that this is the correct behaviour
    if (is_negative(registers.a - data)) {
        registers.p.set_negative_flag();
    }
}

void CPU6502::cpx(uint16_t data_addr)
{
    const uint8_t data = memory.read_byte(data_addr);
    if (registers.x >= data) {
        registers.p.set_carry_bit();
    } else {
        registers.p.clear_carry_flag();
    }
    if (registers.x == data) {
        registers.p.set_zero_flag();
    } else {
        registers.p.clear_zero_flag();
    }

    // TODO: validate that this is the correct behaviour
    if (is_negative(registers.x - data)) {
        registers.p.set_negative_flag();
    }
}

void CPU6502::cpy(uint16_t data_addr)
{
    const uint8_t data = memory.read_byte(data_addr);
    if (registers.y >= data) {
        registers.p.set_carry_bit();
    } else {
        registers.p.clear_carry_flag();
    }
    if (registers.y == data) {
        registers.p.set_zero_flag();
    } else {
        registers.p.clear_zero_flag();
    }

    // TODO: validate that this is the correct behaviour
    if (is_negative(registers.y - data)) {
        registers.p.set_negative_flag();
    }
}

void CPU6502::dec(uint16_t data_addr)
{
    uint8_t data = memory.read_byte(data_addr) - 1;
    memory.write_byte(data_addr, data);

    if (data == 0) {
        registers.p.set_zero_flag();
    }
    if (is_negative(data)) {
        registers.p.set_negative_flag();
    }
}

void CPU6502::dex(uint16_t data_addr)
{
    (void)data_addr;
    registers.x--;
    if (registers.x == 0) {
        registers.p.set_zero_flag();
    }
    if (is_negative(registers.x)) {
        registers.p.set_negative_flag();
    }
}

void CPU6502::dey(uint16_t data_addr)
{
    (void)data_addr;
    registers.y--;
    if (registers.y == 0) {
        registers.p.set_zero_flag();
    }
    if (is_negative(registers.y)) {
        registers.p.set_negative_flag();
    }
}

void CPU6502::eor(uint16_t data_addr)
{
    const uint8_t data = memory.read_byte(data_addr);
    registers.a ^= data;
    if (registers.a == 0) {
        registers.p.set_zero_flag();
    }
    if (is_negative(registers.a)) {
        registers.p.set_negative_flag();
    }
}

void CPU6502::inc(uint16_t data_addr)
{
    uint8_t data = memory.read_byte(data_addr) + 1;
    memory.write_byte(data_addr, data);
    if (data == 0) {
        registers.p.set_zero_flag();
    }
    if (is_negative(data)) {
        registers.p.set_negative_flag();
    }
}
