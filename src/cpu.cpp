#include "cpu.h"

#include "log.h"
#include "opcodes.h"
#include <cstdint>
#include <utility>

static void log_cpu_state(const CPU6502& cpu)
{
    info_message("a: {:02X}  x: {:02X}  y: {:02X}  sp: {:02X}  flags: {}({:02X})  cycles: {:5}",
                 cpu.registers.a,
                 cpu.registers.x,
                 cpu.registers.y,
                 cpu.registers.s,
                 cpu.registers.p,
                 (uint8_t)cpu.registers.p.reg,
                 cpu.cycle_count);
}

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

StatusRegFlag operator|(StatusRegFlag a, StatusRegFlag b)
{
    return (StatusRegFlag)((uint8_t)a | (uint8_t)b);
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
    registers.s = 0xFD;
    registers.p.set_int_disable_flag();
    registers.p.set_bflag();

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

    m_InstructionMap[OPCODE_BIT_ZP]  = { "BIT", &CPU6502::bit, &CPU6502::zp, 3 };
    m_InstructionMap[OPCODE_BIT_ABS] = { "BIT", &CPU6502::bit, &CPU6502::abs, 4 };

    m_InstructionMap[OPCODE_BMI_REL] = { "BMI", &CPU6502::bmi, &CPU6502::rel, 2 };

    m_InstructionMap[OPCODE_BPL_REL] = { "BPL", &CPU6502::bpl, &CPU6502::rel, 2 };

    m_InstructionMap[OPCODE_BRK_IMP] = { "BRK", &CPU6502::brk, &CPU6502::imp, 7 };

    m_InstructionMap[OPCODE_BVC_REL] = { "BVC", &CPU6502::bvc, &CPU6502::rel, 2 };

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
    m_InstructionMap[OPCODE_INX_IMP]  = { "INX", &CPU6502::inx, &CPU6502::imp, 2 };
    m_InstructionMap[OPCODE_INY_IMP]  = { "INY", &CPU6502::iny, &CPU6502::imp, 2 };

    m_InstructionMap[OPCODE_JMP_ABS] = { "JMP", &CPU6502::jmp, &CPU6502::abs, 3 };
    m_InstructionMap[OPCODE_JMP_IND] = { "JMP", &CPU6502::jmp, &CPU6502::ind, 5 };
    m_InstructionMap[OPCODE_JSR_ABS] = { "JSR", &CPU6502::jsr, &CPU6502::abs, 6 };

    m_InstructionMap[OPCODE_LSR_ACC]  = { "LSR", &CPU6502::lsr_acc, nullptr, 2 };
    m_InstructionMap[OPCODE_LSR_ZP]   = { "LSR", &CPU6502::lsr, &CPU6502::zp, 5 };
    m_InstructionMap[OPCODE_LSR_ZPX]  = { "LSR", &CPU6502::lsr, &CPU6502::zpx, 6 };
    m_InstructionMap[OPCODE_LSR_ABS]  = { "LSR", &CPU6502::lsr, &CPU6502::abs, 6 };
    m_InstructionMap[OPCODE_LSR_ABSX] = { "LSR", &CPU6502::lsr, &CPU6502::absx, 7 };

    m_InstructionMap[OPCODE_NOP_IMP] = { "NOP", &CPU6502::nop, &CPU6502::imp, 2 };

    m_InstructionMap[OPCODE_ORA_IMM]  = { "ORA", &CPU6502::ora, &CPU6502::imm, 2 };
    m_InstructionMap[OPCODE_ORA_ZP]   = { "ORA", &CPU6502::ora, &CPU6502::zp, 3 };
    m_InstructionMap[OPCODE_ORA_ZPX]  = { "ORA", &CPU6502::ora, &CPU6502::zpx, 4 };
    m_InstructionMap[OPCODE_ORA_ABS]  = { "ORA", &CPU6502::ora, &CPU6502::abs, 4 };
    m_InstructionMap[OPCODE_ORA_ABSX] = { "ORA", &CPU6502::ora, &CPU6502::absx, 4 };
    m_InstructionMap[OPCODE_ORA_ABSY] = { "ORA", &CPU6502::ora, &CPU6502::absy, 4 };
    m_InstructionMap[OPCODE_ORA_INDX] = { "ORA", &CPU6502::ora, &CPU6502::indx, 6 };
    m_InstructionMap[OPCODE_ORA_INDY] = { "ORA", &CPU6502::ora, &CPU6502::indy, 5 };

    m_InstructionMap[OPCODE_PHA_IMP] = { "PHA", &CPU6502::pha, &CPU6502::imp, 3 };
    m_InstructionMap[OPCODE_PHP_IMP] = { "PHP", &CPU6502::php, &CPU6502::imp, 3 };
    m_InstructionMap[OPCODE_PLA_IMP] = { "PLA", &CPU6502::pla, &CPU6502::imp, 4 };
    m_InstructionMap[OPCODE_PLP_IMP] = { "PLP", &CPU6502::plp, &CPU6502::imp, 4 };

    m_InstructionMap[OPCODE_ROL_ACC]  = { "ROL", &CPU6502::rol_acc, nullptr, 2 };
    m_InstructionMap[OPCODE_ROL_ZP]   = { "ROL", &CPU6502::rol, &CPU6502::zp, 5 };
    m_InstructionMap[OPCODE_ROL_ZPX]  = { "ROL", &CPU6502::rol, &CPU6502::zpx, 6 };
    m_InstructionMap[OPCODE_ROL_ABS]  = { "ROL", &CPU6502::rol, &CPU6502::abs, 6 };
    m_InstructionMap[OPCODE_ROL_ABSX] = { "ROL", &CPU6502::rol, &CPU6502::absx, 7 };

    m_InstructionMap[OPCODE_ROR_ACC]  = { "ROR", &CPU6502::ror_acc, nullptr, 2 };
    m_InstructionMap[OPCODE_ROR_ZP]   = { "ROR", &CPU6502::ror, &CPU6502::zp, 5 };
    m_InstructionMap[OPCODE_ROR_ZPX]  = { "ROR", &CPU6502::ror, &CPU6502::zpx, 6 };
    m_InstructionMap[OPCODE_ROR_ABS]  = { "ROR", &CPU6502::ror, &CPU6502::abs, 6 };
    m_InstructionMap[OPCODE_ROR_ABSX] = { "ROR", &CPU6502::ror, &CPU6502::absx, 7 };

    m_InstructionMap[OPCODE_RTI_IMP] = { "RTI", &CPU6502::rti, &CPU6502::imp, 6 };
    m_InstructionMap[OPCODE_RTS_IMP] = { "RTS", &CPU6502::rts, &CPU6502::imp, 6 };

    m_InstructionMap[OPCODE_SBC_IMM]  = { "SBC", &CPU6502::sbc, &CPU6502::imm, 2 };
    m_InstructionMap[OPCODE_SBC_ZP]   = { "SBC", &CPU6502::sbc, &CPU6502::zp, 3 };
    m_InstructionMap[OPCODE_SBC_ZPX]  = { "SBC", &CPU6502::sbc, &CPU6502::zpx, 4 };
    m_InstructionMap[OPCODE_SBC_ABS]  = { "SBC", &CPU6502::sbc, &CPU6502::abs, 4 };
    m_InstructionMap[OPCODE_SBC_ABSX] = { "SBC", &CPU6502::sbc, &CPU6502::absx, 4 };
    m_InstructionMap[OPCODE_SBC_ABSY] = { "SBC", &CPU6502::sbc, &CPU6502::absy, 4 };
    m_InstructionMap[OPCODE_SBC_INDX] = { "SBC", &CPU6502::sbc, &CPU6502::indx, 6 };
    m_InstructionMap[OPCODE_SBC_INDY] = { "SBC", &CPU6502::sbc, &CPU6502::indy, 5 };

    m_InstructionMap[OPCODE_SEC_IMP] = { "SEC", &CPU6502::sec, &CPU6502::imp, 2 };
    m_InstructionMap[OPCODE_SED_IMP] = { "SED", &CPU6502::sed, &CPU6502::imp, 2 };
    m_InstructionMap[OPCODE_SEI_IMP] = { "SEI", &CPU6502::sei, &CPU6502::imp, 2 };

    m_InstructionMap[OPCODE_STA_ZP]   = { "STA", &CPU6502::sta, &CPU6502::zp, 3 };
    m_InstructionMap[OPCODE_STA_ZPX]  = { "STA", &CPU6502::sta, &CPU6502::zpx, 4 };
    m_InstructionMap[OPCODE_STA_ABS]  = { "STA", &CPU6502::sta, &CPU6502::abs, 4 };
    m_InstructionMap[OPCODE_STA_ABSX] = { "STA", &CPU6502::sta, &CPU6502::absx, 5 };
    m_InstructionMap[OPCODE_STA_ABSY] = { "STA", &CPU6502::sta, &CPU6502::absy, 5 };
    m_InstructionMap[OPCODE_STA_INDX] = { "STA", &CPU6502::sta, &CPU6502::indx, 6 };
    m_InstructionMap[OPCODE_STA_INDY] = { "STA", &CPU6502::sta, &CPU6502::indy, 6 };
    m_InstructionMap[OPCODE_STX_ZP]   = { "STX", &CPU6502::stx, &CPU6502::zp, 3 };
    m_InstructionMap[OPCODE_STX_ZPY]  = { "STX", &CPU6502::stx, &CPU6502::zpy, 4 };
    m_InstructionMap[OPCODE_STX_ABS]  = { "STX", &CPU6502::stx, &CPU6502::abs, 4 };
    m_InstructionMap[OPCODE_STY_ZP]   = { "STY", &CPU6502::sty, &CPU6502::zp, 3 };
    m_InstructionMap[OPCODE_STY_ZPX]  = { "STY", &CPU6502::sty, &CPU6502::zpx, 4 };
    m_InstructionMap[OPCODE_STY_ABS]  = { "STY", &CPU6502::sty, &CPU6502::abs, 4 };

    m_InstructionMap[OPCODE_TAX_IMP] = { "TAX", &CPU6502::tax, &CPU6502::imp, 2 };
    m_InstructionMap[OPCODE_TAY_IMP] = { "TAY", &CPU6502::tay, &CPU6502::imp, 2 };
    m_InstructionMap[OPCODE_TSX_IMP] = { "TSX", &CPU6502::tsx, &CPU6502::imp, 2 };
    m_InstructionMap[OPCODE_TXA_IMP] = { "TXA", &CPU6502::txa, &CPU6502::imp, 2 };
    m_InstructionMap[OPCODE_TXS_IMP] = { "TXS", &CPU6502::txs, &CPU6502::imp, 2 };
    m_InstructionMap[OPCODE_TYA_IMP] = { "TYA", &CPU6502::tya, &CPU6502::imp, 2 };
}

void CPU6502::next_cycle()
{
    if (cycles_remaining > 0) {
        cycles_remaining--;
        cycle_count++;
        return;
    }

    cycles_remaining = process_instruction() - 1;
    cycle_count++;
}

uint8_t CPU6502::process_instruction()
{
    const uint8_t opcode  = memory.read_byte(registers.pc++);
    const auto handler_it = m_InstructionMap.find(opcode);
    if (handler_it == m_InstructionMap.end()) {
        NOT_IMPLEMENTED();
    }

    if (verbose_log) {
        fmt::print(stderr, "{:4X} {:4} {:02X}  ", registers.pc - 1, handler_it->second.name, opcode);
        log_cpu_state(*this);
    }

    auto op                  = handler_it->second.operation_fn;
    auto addr                = handler_it->second.addressing_fn;
    const uint16_t data_addr = addr ? (this->*addr)() : 0;
    return (this->*op)(data_addr) + handler_it->second.cycles;
}

void CPU6502::load_prg_rom(std::span<const uint8_t> buf)
{
    // TODO: validate size of data passed in
    memory.write_rom(0x8000, buf);
    if (buf.size() == 16 * 1024) {
        memory.write_rom(0xC000, buf);
    }
}

void CPU6502::reset()
{
    registers.pc = memory.read_word(0xFFFC);
    cycle_count += 7;
}

void CPU6502::stack_push_byte(uint8_t data)
{
    memory.write_byte(registers.s + 0x100, data);
    registers.s--;
}

void CPU6502::stack_push_word(uint16_t data)
{
    stack_push_byte((data >> 8) & 0xFF);
    stack_push_byte(data & 0xFF);
}

uint8_t CPU6502::stack_top_byte() const
{
    return memory.read_byte(registers.s + 0x100 + 1);
}

uint16_t CPU6502::stack_top_word() const
{
    const uint8_t lo_byte = memory.read_byte(registers.s + 0x100 + 1);
    const uint8_t hi_byte = memory.read_byte(registers.s + 0x100 + 2);
    return (hi_byte << 8) | lo_byte;
}

uint8_t CPU6502::stack_pop_byte()
{
    uint8_t val = stack_top_byte();
    registers.s++;
    return val;
}

uint16_t CPU6502::stack_pop_word()
{
    uint16_t val = stack_top_word();
    registers.s += 2;
    return val;
}

void CPU6502::adjust_zero_and_negative_flags(uint8_t data)
{
    if (data == 0) {
        registers.p.set_zero_flag();
    } else {
        registers.p.clear_zero_flag();
    }

    if (is_negative(data)) {
        registers.p.set_negative_flag();
    } else {
        registers.p.clear_negative_flag();
    }
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
    const uint16_t ret = memory.read_word(registers.pc);
    registers.pc += 2;
    return ret;
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

uint16_t CPU6502::ind()
{
    const uint16_t addr = memory.read_word(registers.pc);
    registers.pc += 2;
    return memory.read_word(addr);
}

uint8_t CPU6502::lda(uint16_t data_addr)
{
    registers.a = memory.read_byte(data_addr);
    adjust_zero_and_negative_flags(registers.a);
    return 0;
}

uint8_t CPU6502::ldx(uint16_t data_addr)
{
    registers.x = memory.read_byte(data_addr);
    adjust_zero_and_negative_flags(registers.x);
    return 0;
}

uint8_t CPU6502::ldy(uint16_t data_addr)
{
    registers.y = memory.read_byte(data_addr);
    adjust_zero_and_negative_flags(registers.y);
    return 0;
}

uint8_t CPU6502::adc(uint16_t data_addr)
{
    const uint8_t data    = memory.read_byte(data_addr);
    const uint16_t res    = registers.a + data + registers.p.carry_bit_set();
    const uint8_t old_acc = registers.a;
    registers.a           = res & 0xFF;
    adjust_zero_and_negative_flags(registers.a);
    if (res & 0xFF00) {
        registers.p.set_carry_bit();
    } else {
        registers.p.clear_carry_flag();
    }
    if (is_positive(data) && is_positive(old_acc) && is_negative(res)) {
        registers.p.set_overflow_bit();
    } else if (is_negative(data) && is_negative(old_acc) && is_positive(res)) {
        registers.p.set_overflow_bit();
    } else {
        registers.p.clear_overflow_flag();
    }
    return 0;
}

uint8_t CPU6502::and_op(uint16_t data_addr)
{
    const uint8_t data = memory.read_byte(data_addr);
    registers.a &= data;
    if (registers.a == 0) {
        registers.p.set_zero_flag();
    }
    if (is_negative(registers.a)) {
        registers.p.set_negative_flag();
    }

    return 0;
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

uint8_t CPU6502::asl(uint16_t data_addr)
{
    uint8_t data = memory.read_byte(data_addr);
    data         = asl_impl(data);
    memory.write_byte(data_addr, data);

    return 0;
}

uint8_t CPU6502::asl_acc(uint16_t data_addr)
{
    (void)data_addr; // this instruction operates directly on the accumulator
    registers.a = asl_impl(registers.a);

    return 0;
}

uint8_t CPU6502::displace_pc_from_data_addr(uint16_t data_addr)
{
    const uint16_t old_page = registers.pc & 0xFF00;
    uint8_t data            = memory.read_byte(data_addr);
    if (is_positive(data)) {
        registers.pc += data;
    } else {
        data = twos_compliment_flip(data);
        registers.pc -= data;
    }
    const uint16_t new_page = registers.pc & 0xFF00;
    if (old_page != new_page) return 2;
    return 1;
}

uint8_t CPU6502::bcc(uint16_t data_addr)
{
    if (!registers.p.carry_bit_set()) {
        return displace_pc_from_data_addr(data_addr);
    }
    return 0;
}

uint8_t CPU6502::bcs(uint16_t data_addr)
{
    if (registers.p.carry_bit_set()) {
        return displace_pc_from_data_addr(data_addr);
    }
    return 0;
}

uint8_t CPU6502::beq(uint16_t data_addr)
{
    if (registers.p.zero_flag_set()) {
        return displace_pc_from_data_addr(data_addr);
    }
    return 0;
}

uint8_t CPU6502::bne(uint16_t data_addr)
{
    if (!registers.p.zero_flag_set()) {
        return displace_pc_from_data_addr(data_addr);
    }
    return 0;
}

uint8_t CPU6502::bit(uint16_t data_addr)
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
    return 0;
}

uint8_t CPU6502::bmi(uint16_t data_addr)
{
    if (registers.p.negative_flag_set()) {
        return displace_pc_from_data_addr(data_addr);
    }
    return 0;
}

uint8_t CPU6502::bpl(uint16_t data_addr)
{
    if (!registers.p.negative_flag_set()) {
        return displace_pc_from_data_addr(data_addr);
    }
    return 0;
}

uint8_t CPU6502::brk(uint16_t data_addr)
{
    (void)data_addr;
    NOT_IMPLEMENTED();
    return 0;
}

uint8_t CPU6502::bvc(uint16_t data_addr)
{
    if (!registers.p.overflow_flag_set()) {
        return displace_pc_from_data_addr(data_addr);
    }
    return 0;
}

uint8_t CPU6502::bvs(uint16_t data_addr)
{
    if (registers.p.overflow_flag_set()) {
        return displace_pc_from_data_addr(data_addr);
    }
    return 0;
}

uint8_t CPU6502::clc(uint16_t data_addr)
{
    (void)data_addr;
    registers.p.clear_carry_flag();
    return 0;
}

uint8_t CPU6502::cld(uint16_t)
{
    registers.p.clear_decimal_flag();
    return 0;
}

uint8_t CPU6502::cli(uint16_t data_addr)
{
    (void)data_addr;
    registers.p.clear_int_disable_flag();
    return 0;
}

uint8_t CPU6502::clv(uint16_t data_addr)
{
    (void)data_addr;
    registers.p.clear_overflow_flag();
    return 0;
}

static void set_flags_from_compare(uint8_t reg, uint8_t mem_data, StatusRegister& flags)
{
    if (reg >= mem_data) {
        flags.set_carry_bit();
    } else {
        flags.clear_carry_flag();
    }
    if (reg == mem_data) {
        flags.set_zero_flag();
    } else {
        flags.clear_zero_flag();
    }

    if (is_negative(reg - mem_data)) {
        flags.set_negative_flag();
    } else {
        flags.clear_negative_flag();
    }
}

uint8_t CPU6502::cmp(uint16_t data_addr)
{
    const uint8_t data = memory.read_byte(data_addr);
    set_flags_from_compare(registers.a, data, registers.p);
    return 0;
}

uint8_t CPU6502::cpx(uint16_t data_addr)
{
    const uint8_t data = memory.read_byte(data_addr);
    set_flags_from_compare(registers.x, data, registers.p);
    return 0;
}

uint8_t CPU6502::cpy(uint16_t data_addr)
{
    const uint8_t data = memory.read_byte(data_addr);
    set_flags_from_compare(registers.y, data, registers.p);
    return 0;
}

uint8_t CPU6502::dec(uint16_t data_addr)
{
    uint8_t data = memory.read_byte(data_addr) - 1;
    memory.write_byte(data_addr, data);

    if (data == 0) {
        registers.p.set_zero_flag();
    }
    if (is_negative(data)) {
        registers.p.set_negative_flag();
    }
    return 0;
}

uint8_t CPU6502::dex(uint16_t)
{
    registers.x--;
    adjust_zero_and_negative_flags(registers.x);
    return 0;
}

uint8_t CPU6502::dey(uint16_t)
{
    registers.y--;
    adjust_zero_and_negative_flags(registers.y);
    return 0;
}

uint8_t CPU6502::eor(uint16_t data_addr)
{
    const uint8_t data = memory.read_byte(data_addr);
    registers.a ^= data;
    adjust_zero_and_negative_flags(registers.a);
    return 0;
}

uint8_t CPU6502::inc(uint16_t data_addr)
{
    uint8_t data = memory.read_byte(data_addr) + 1;
    memory.write_byte(data_addr, data);
    adjust_zero_and_negative_flags(data);
    return 0;
}

uint8_t CPU6502::inx(uint16_t)
{
    registers.x++;
    adjust_zero_and_negative_flags(registers.x);
    return 0;
}

uint8_t CPU6502::iny(uint16_t)
{
    registers.y++;
    adjust_zero_and_negative_flags(registers.y);
    return 0;
}

uint8_t CPU6502::jmp(uint16_t data_addr)
{
    registers.pc = data_addr;
    return 0;
}

uint8_t CPU6502::jsr(uint16_t data_addr)
{
    stack_push_word(registers.pc - 1);
    registers.pc = data_addr;
    return 0;
}

uint8_t CPU6502::lsr(uint16_t data_addr)
{
    uint8_t data        = memory.read_byte(data_addr);
    const uint8_t carry = data & 1;
    data >>= 1;
    memory.write_byte(data_addr, data);
    if (carry) {
        registers.p.set_carry_bit();
    }
    return 0;
}

uint8_t CPU6502::lsr_acc(uint16_t data_addr)
{
    (void)data_addr;
    const uint8_t carry = registers.a & 1;
    registers.a >>= 1;
    if (carry) {
        registers.p.set_carry_bit();
    }
    return 0;
}

uint8_t CPU6502::nop(uint16_t data_addr)
{
    (void)data_addr;
    return 0;
}

uint8_t CPU6502::ora(uint16_t data_addr)
{
    const uint8_t data = memory.read_byte(data_addr);
    registers.a |= data;
    if (registers.a == 0) {
        registers.p.set_zero_flag();
    }
    if (is_negative(registers.a)) {
        registers.p.set_negative_flag();
    }
    return 0;
}

uint8_t CPU6502::pha(uint16_t data_addr)
{
    (void)data_addr;
    stack_push_byte(registers.a);
    return 0;
}

uint8_t CPU6502::php(uint16_t)
{
    // push status register with the break flag and bit 5 set to 1
    StatusRegister to_push = registers.p;
    to_push.set_bflag();
    to_push.set_unused_flag();
    stack_push_byte((uint8_t)to_push.reg);
    return 0;
}

uint8_t CPU6502::pla(uint16_t)
{
    registers.a = stack_pop_byte();
    adjust_zero_and_negative_flags(registers.a);
    return 0;
}

void CPU6502::pop_p_from_stack()
{
    // When we pull the break flag and bit 5 should be ignored
    // So restore these to whatever state they were in before this
    const bool bflag       = registers.p.bflag_flag_set();
    const bool unused_flag = registers.p.unused_flag_set();
    registers.p.reg        = (StatusRegFlag)stack_pop_byte();
    bflag ? registers.p.set_bflag() : registers.p.clear_bflag();
    unused_flag ? registers.p.set_unused_flag() : registers.p.clear_unused_flag();
}

uint8_t CPU6502::plp(uint16_t)
{
    pop_p_from_stack();
    return 0;
}

uint8_t CPU6502::rol(uint16_t data_addr)
{
    uint8_t data = memory.read_byte(data_addr);
    data         = rol_impl(data);
    memory.write_byte(data_addr, data);
    return 0;
}

uint8_t CPU6502::rol_acc(uint16_t data_addr)
{
    (void)data_addr;
    registers.a = rol_impl(registers.a);
    return 0;
}

uint8_t CPU6502::rol_impl(uint8_t data)
{
    const uint8_t old_carry = registers.p.carry_bit_set();

    data <<= 1;
    if (old_carry) {
        data |= (uint8_t)1;
    } else {
        data &= ~((uint8_t)1);
    }

    return data;
}

uint8_t CPU6502::ror(uint16_t data_addr)
{
    uint8_t data = memory.read_byte(data_addr);
    data         = ror_impl(data);
    memory.write_byte(data_addr, data);
    return 0;
}

uint8_t CPU6502::ror_acc(uint16_t data_addr)
{
    (void)data_addr;
    registers.a = ror_impl(registers.a);
    return 0;
}

uint8_t CPU6502::ror_impl(uint8_t data)
{
    const uint8_t old_carry = registers.p.carry_bit_set();

    data >>= 1;
    if (old_carry) {
        data |= (uint8_t)(1 << 7);
    } else {
        data &= ~((uint8_t)(1 << 7));
    }

    return data;
}

uint8_t CPU6502::rti(uint16_t)
{
    pop_p_from_stack();
    registers.pc = stack_pop_word();
    return 0;
}

uint8_t CPU6502::rts(uint16_t data_addr)
{
    (void)data_addr;
    registers.pc = stack_pop_word() + 1;
    return 0;
}

uint8_t CPU6502::sbc(uint16_t data_addr)
{
    const uint8_t data    = memory.read_byte(data_addr);
    const uint16_t res    = registers.a - data - ((uint8_t)1 - (uint8_t)registers.p.carry_bit_set());
    const uint8_t old_acc = registers.a;
    registers.a           = res & 0xFF;
    if (res > 0xFF) {
        registers.p.clear_carry_flag();
    } else {
        registers.p.set_carry_bit();
    }
    if (is_negative(old_acc) && is_positive(data) && is_positive(registers.a)) {
        registers.p.set_overflow_bit();
    } else if (is_positive(old_acc) && is_negative(data) && is_negative(registers.a)) {
        registers.p.set_overflow_bit();
    } else {
        registers.p.clear_overflow_flag();
    }
    adjust_zero_and_negative_flags(registers.a);
    return 0;
}

uint8_t CPU6502::sec(uint16_t)
{
    registers.p.set_carry_bit();
    return 0;
}

uint8_t CPU6502::sed(uint16_t)
{
    registers.p.set_decimal_flag();
    return 0;
}

uint8_t CPU6502::sei(uint16_t)
{
    registers.p.set_int_disable_flag();
    return 0;
}

uint8_t CPU6502::sta(uint16_t data_addr)
{
    memory.write_byte(data_addr, registers.a);
    return 0;
}

uint8_t CPU6502::stx(uint16_t data_addr)
{
    memory.write_byte(data_addr, registers.x);
    return 0;
}

uint8_t CPU6502::sty(uint16_t data_addr)
{
    memory.write_byte(data_addr, registers.y);
    return 0;
}

uint8_t CPU6502::tax(uint16_t)
{
    registers.x = registers.a;
    adjust_zero_and_negative_flags(registers.x);
    return 0;
}

uint8_t CPU6502::tay(uint16_t)
{
    registers.y = registers.a;
    adjust_zero_and_negative_flags(registers.y);
    return 0;
}

uint8_t CPU6502::tsx(uint16_t)
{
    registers.x = registers.s;
    adjust_zero_and_negative_flags(registers.x);
    return 0;
}

uint8_t CPU6502::txa(uint16_t)
{
    registers.a = registers.x;
    adjust_zero_and_negative_flags(registers.a);
    return 0;
}

uint8_t CPU6502::txs(uint16_t)
{
    registers.s = registers.x;
    return 0;
}

uint8_t CPU6502::tya(uint16_t)
{
    registers.a = registers.y;
    adjust_zero_and_negative_flags(registers.a);
    return 0;
}
