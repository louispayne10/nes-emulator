#include "cpu.h"

#include "log.h"
#include "opcodes.h"
#include <utility>

constexpr uint8_t BIT_7 = (1 << 7);

CPU6502::CPU6502()
{
    m_InstructionMap.insert(std::make_pair(OPCODE_LDA_IMM, &CPU6502::lda_imm));
    m_InstructionMap.insert(std::make_pair(OPCODE_LDA_ZP, &CPU6502::lda_zp));
    m_InstructionMap.insert(std::make_pair(OPCODE_LDA_ZPX, &CPU6502::lda_zpx));
    m_InstructionMap.insert(std::make_pair(OPCODE_LDA_ABS, &CPU6502::lda_abs));
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

/* Add with carry */
void adc() {}

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

uint8_t CPU6502::abs()
{
    const uint16_t instruction_offset = memory.read_word(registers.pc);
    registers.pc += 2;
    return memory.read_byte(instruction_offset);
}

void CPU6502::lda_imm()
{
    const uint8_t data = imm();
    registers.a        = data;
    if (data == 0) {
        registers.p.set_zero_flag();
    }
    if (data & BIT_7) {
        registers.p.set_negative_flag();
    }
}

void CPU6502::lda_zp()
{
    const uint8_t data = zp();
    if (data == 0) {
        registers.p.set_zero_flag();
    }
    if (data & BIT_7) {
        registers.p.set_negative_flag();
    }
    registers.a = data;
}

void CPU6502::lda_zpx()
{
    const uint8_t data = zpx();
    if (data == 0) {
        registers.p.set_zero_flag();
    }
    if (data & BIT_7) {
        registers.p.set_negative_flag();
    }
    registers.a = data;
}

void CPU6502::lda_abs()
{
    const uint8_t data = abs();
    if (data == 0) {
        registers.p.set_zero_flag();
    }
    if (data & BIT_7) {
        registers.p.set_negative_flag();
    }
    registers.a = data;
}
