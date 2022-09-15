#pragma once

#include "memory.h"
#include <array>
#include <cstdint>
#include <string>
#include <unordered_map>

struct Clock
{
    uint64_t cycle_count;
};

struct StatusRegister
{
    uint8_t reg;

    static constexpr uint8_t CARRY_MASK       = (1 << 0);
    static constexpr uint8_t ZERO_MASK        = (1 << 1);
    static constexpr uint8_t INT_DISABLE_MASK = (1 << 3);
    static constexpr uint8_t DECIMAL_MASK     = (1 << 4);
    static constexpr uint8_t BFLAG_MASK       = (1 << 5);
    static constexpr uint8_t OVERFLOW_MASK    = (1 << 6);
    static constexpr uint8_t NEGATIVE_MASK    = (1 << 7);

    void set_zero_flag() { reg |= ZERO_MASK; }
    void set_negative_flag() { reg |= NEGATIVE_MASK; }

    bool zero_flag_set() { return reg & ZERO_MASK; }
    bool negative_flag_set() { return reg & NEGATIVE_MASK; }

    friend auto operator<=>(StatusRegister, StatusRegister) = default;
};

struct CpuRegisters
{
    uint8_t a; // accumulator
    uint8_t x;
    uint8_t y;
    uint16_t pc;      // program counter
    uint8_t s;        // the stack pointer
    StatusRegister p; // the status register
};

/* TODO: execute the instruction AFTER the cycles to complete it are over
 * this will be a slightly more accurate emulation
 * Of course real 6502 CPUs will have some kind of microcode going on
 * during those cycles but we'll just wait
 */
class CPU6502
{
public:
    CPU6502();

    CpuRegisters registers = {};
    Memory memory;

    void process_instruction();

    // addressing modes
    uint8_t imm();
    uint8_t zp();
    uint8_t zpx();
    uint8_t abs();
    uint8_t absx();

    // specific instructions
    void lda_imm();
    void lda_zp();
    void lda_zpx();
    void lda_abs();
    void lda_absx();

    // generic instructions
    void lda(uint8_t data);

    // TODO: Move this to a table once we can
    using instruction_fn = void (CPU6502::*)();
    std::unordered_map<uint8_t, instruction_fn> m_InstructionMap;

    struct Instruction
    {
        std::string name;
        /* addressing mode function */
        /* execute instruction function */
        /* some information on clock cycles */
    };
    // std::array<Instruction, 256> m_InstructionTable;
};
