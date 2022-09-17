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


enum class StatusRegFlag : uint8_t {
    CARRY_MASK       = (1 << 0),
    ZERO_MASK        = (1 << 1),
    INT_DISABLE_MASK = (1 << 3),
    DECIMAL_MASK     = (1 << 4),
    BFLAG_MASK       = (1 << 5),
    OVERFLOW_MASK    = (1 << 6),
    NEGATIVE_MASK    = (1 << 7)
};
StatusRegFlag& operator|=(StatusRegFlag& a, StatusRegFlag b);
bool operator&(StatusRegFlag a, StatusRegFlag b);

struct StatusRegister
{
    StatusRegFlag reg;

    void set_zero_flag() { reg |= StatusRegFlag::ZERO_MASK; }
    void set_negative_flag() { reg |= StatusRegFlag::NEGATIVE_MASK; }

    bool zero_flag_set() { return reg & StatusRegFlag::ZERO_MASK; }
    bool negative_flag_set() { return reg & StatusRegFlag::NEGATIVE_MASK; }

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
    uint8_t zpy(); // TODO: add test for this
    uint8_t abs();
    uint8_t absx();
    uint8_t absy();
    uint8_t indx();
    uint8_t indy();

    // specific instructions
    void lda_imm();
    void lda_zp();
    void lda_zpx();
    void lda_abs();
    void lda_absx();
    void lda_absy();
    void lda_indx();
    void lda_indy();

    void ldx_imm();
    void ldx_zp();
    void ldx_zpy();
    void ldx_abs();
    void ldx_absy();

    // generic instructions
    void lda(uint8_t data);
    void ldx(uint8_t data);

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
