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
    Carry      = (1 << 0),
    Zero       = (1 << 1),
    IntDisable = (1 << 3),
    Decimal    = (1 << 4),
    BFlag      = (1 << 5),
    Overflow   = (1 << 6),
    Negative   = (1 << 7)
};
StatusRegFlag& operator|=(StatusRegFlag& a, StatusRegFlag b);
bool operator&(StatusRegFlag a, StatusRegFlag b);

struct StatusRegister
{
    StatusRegFlag reg;

    void set_zero_flag() { reg |= StatusRegFlag::Zero; }
    bool zero_flag_set() const { return reg & StatusRegFlag::Zero; }

    void set_negative_flag() { reg |= StatusRegFlag::Negative; }
    bool negative_flag_set() const { return reg & StatusRegFlag::Negative; }

    void set_carry_bit() { reg |= StatusRegFlag::Carry; }
    bool carry_bit_set() const { return reg & StatusRegFlag::Carry; }

    void set_overflow_bit() { reg |= StatusRegFlag::Overflow; }
    bool overflow_flag_set() const { return reg & StatusRegFlag::Overflow; }
};
bool operator==(StatusRegister lhs, StatusRegister rhs);

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
    uint16_t imm();
    uint16_t zp();
    uint16_t zpx();
    uint16_t zpy();
    uint16_t abs();
    uint16_t absx();
    uint16_t absy();
    uint16_t indx();
    uint16_t indy();

    // generic instructions
    void lda(uint16_t data_addr);
    void ldx(uint16_t data_data);
    void ldy(uint16_t data_data);
    void adc(uint16_t data_data);
    void and_op(uint16_t data_addr); // can't just name this function 'and' as that is a c++ keyword

    // TODO: Move this to a lookup table once we can
    using operation_fn_t  = void (CPU6502::*)(uint16_t);
    using addressing_fn_t = uint16_t (CPU6502::*)();
    struct Instruction
    {
        std::string name;
        operation_fn_t operation_fn;
        addressing_fn_t addressing_fn;
    };
    std::unordered_map<uint8_t, Instruction> m_InstructionMap;
};
