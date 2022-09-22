#pragma once

#include "memory.h"
#include <array>
#include <cstdint>
#include <string>
#include <unordered_map>

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
StatusRegFlag& operator&=(StatusRegFlag& a, StatusRegFlag b);
bool operator&(StatusRegFlag a, StatusRegFlag b);
StatusRegFlag operator~(StatusRegFlag a);

struct StatusRegister
{
    StatusRegFlag reg;

    void set_zero_flag() { reg |= StatusRegFlag::Zero; }
    void clear_zero_flag() { reg &= ~StatusRegFlag::Zero; }
    bool zero_flag_set() const { return reg & StatusRegFlag::Zero; }

    void set_negative_flag() { reg |= StatusRegFlag::Negative; }
    void clear_negative_flag() { reg &= ~StatusRegFlag::Negative; }
    bool negative_flag_set() const { return reg & StatusRegFlag::Negative; }

    void set_carry_bit() { reg |= StatusRegFlag::Carry; }
    void clear_carry_flag() { reg &= ~StatusRegFlag::Carry; }
    bool carry_bit_set() const { return reg & StatusRegFlag::Carry; }

    void set_overflow_bit() { reg |= StatusRegFlag::Overflow; }
    void clear_overflow_flag() { reg &= ~StatusRegFlag::Overflow; }
    bool overflow_flag_set() const { return reg & StatusRegFlag::Overflow; }

    void set_int_disable_flag() { reg |= StatusRegFlag::IntDisable; }
    void clear_int_disable_flag() { reg &= ~StatusRegFlag::IntDisable; }
    bool int_disable_flag_set() const { return reg & StatusRegFlag::IntDisable; }
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

// TODO: implement extra cycle for page boundary being crossed
class CPU6502
{
public:
    CPU6502();

    CpuRegisters registers = {};
    Memory memory;

    void next_cycle();
    uint8_t cycles_remaining = 0;
    uint8_t process_instruction();

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
    uint16_t rel();
    uint16_t imp();
    uint16_t ind();

    // generic instructions
    void lda(uint16_t data_addr);
    void ldx(uint16_t data_data);
    void ldy(uint16_t data_data);
    void adc(uint16_t data_data);
    void and_op(uint16_t data_addr); // can't just name this function 'and' as that is a c++ keyword
    void asl(uint16_t data_addr);
    void bcc(uint16_t data_addr);
    void bcs(uint16_t data_addr);
    void beq(uint16_t data_addr);
    void bne(uint16_t data_addr);
    void bit(uint16_t data_addr);
    void bmi(uint16_t data_addr);
    void bpl(uint16_t data_addr);
    void brk(uint16_t data_addr);
    void bvc(uint16_t data_addr);
    void bvs(uint16_t data_addr);
    void clc(uint16_t data_addr);
    void cld(uint16_t data_addr);
    void cli(uint16_t data_addr);
    void clv(uint16_t data_addr);
    void cmp(uint16_t data_addr);
    void cpx(uint16_t data_addr);
    void cpy(uint16_t data_addr);
    void dec(uint16_t data_addr);
    void dex(uint16_t data_addr);
    void dey(uint16_t data_addr);
    void eor(uint16_t data_addr);
    void inc(uint16_t data_addr);
    void inx(uint16_t data_addr);
    void iny(uint16_t data_addr);
    void jmp(uint16_t data_addr);
    void jsr(uint16_t data_addr);
    void lsr(uint16_t data_addr);
    void nop(uint16_t data_addr);
    void ora(uint16_t data_addr);
    void pha(uint16_t data_addr);
    void php(uint16_t data_addr);
    void pla(uint16_t data_addr);

    // helpers
    void asl_acc(uint16_t data_addr);
    uint8_t asl_impl(uint8_t data);
    void lsr_acc(uint16_t data_addr);
    void displace_pc_from_data_addr(uint16_t data_addr);
    void push_stack(uint8_t data);
    uint8_t stack_top() const;
    uint8_t stack_pop();

    // TODO: Move this to a lookup table once we can
    using operation_fn_t  = void (CPU6502::*)(uint16_t);
    using addressing_fn_t = uint16_t (CPU6502::*)();
    struct Instruction
    {
        std::string name;
        operation_fn_t operation_fn;
        addressing_fn_t addressing_fn;
        uint8_t cycles;
    };
    std::unordered_map<uint8_t, Instruction> m_InstructionMap;
};
