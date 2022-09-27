#pragma once

#include "memory.h"

#include <fmt/format.h>

#include <array>
#include <cstdint>
#include <span>
#include <string>
#include <unordered_map>

enum class StatusRegFlag : uint8_t {
    Carry      = (1 << 0),
    Zero       = (1 << 1),
    IntDisable = (1 << 2),
    Decimal    = (1 << 3),
    BFlag      = (1 << 5),
    Overflow   = (1 << 6),
    Negative   = (1 << 7)
};
StatusRegFlag& operator|=(StatusRegFlag& a, StatusRegFlag b);
StatusRegFlag& operator&=(StatusRegFlag& a, StatusRegFlag b);
bool operator&(StatusRegFlag a, StatusRegFlag b);
StatusRegFlag operator|(StatusRegFlag a, StatusRegFlag b);
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

    void set_decimal_flag() { reg |= StatusRegFlag::Decimal; }
    void clear_decimal_flag() { reg &= ~StatusRegFlag::Decimal; }
    bool decimal_flag_set() const { return reg & StatusRegFlag::Decimal; }

    bool bflag_flag_set() const { return reg & StatusRegFlag::BFlag; }
    void set_bflag() { reg |= StatusRegFlag::BFlag; }
};
bool operator==(StatusRegister lhs, StatusRegister rhs);

template <>
struct fmt::formatter<StatusRegister>
{
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(StatusRegister reg, FormatContext& ctx) const -> decltype(ctx.out())
    {
        return fmt::format_to(ctx.out(),
                              "{}{}{}{}{}{}{}",
                              reg.negative_flag_set() ? 'N' : '-',
                              reg.overflow_flag_set() ? 'O' : '-',
                              reg.bflag_flag_set() ? 'B' : '-',
                              reg.decimal_flag_set() ? 'D' : '-',
                              reg.int_disable_flag_set() ? 'I' : '-',
                              reg.zero_flag_set() ? 'Z' : '-',
                              reg.carry_bit_set() ? 'C' : '-');
    }
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

// TODO: implement extra cycle for page boundary being crossed
class CPU6502
{
public:
    CPU6502();

    CpuRegisters registers = {};
    Memory memory;

    void next_cycle();

    uint8_t cycles_remaining = 0;
    uint64_t cycle_count     = 0;

    uint8_t process_instruction();

    void load_prg_rom(std::span<const uint8_t> buf);
    void reset();

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
    uint8_t lda(uint16_t data_addr);
    uint8_t ldx(uint16_t data_data);
    uint8_t ldy(uint16_t data_data);
    uint8_t adc(uint16_t data_data);
    uint8_t and_op(uint16_t data_addr); // can't just name this function 'and' as that is a c++ keyword
    uint8_t asl(uint16_t data_addr);
    uint8_t bcc(uint16_t data_addr);
    uint8_t bcs(uint16_t data_addr);
    uint8_t beq(uint16_t data_addr);
    uint8_t bne(uint16_t data_addr);
    uint8_t bit(uint16_t data_addr);
    uint8_t bmi(uint16_t data_addr);
    uint8_t bpl(uint16_t data_addr);
    uint8_t brk(uint16_t data_addr);
    uint8_t bvc(uint16_t data_addr);
    uint8_t bvs(uint16_t data_addr);
    uint8_t clc(uint16_t data_addr);
    uint8_t cld(uint16_t data_addr);
    uint8_t cli(uint16_t data_addr);
    uint8_t clv(uint16_t data_addr);
    uint8_t cmp(uint16_t data_addr);
    uint8_t cpx(uint16_t data_addr);
    uint8_t cpy(uint16_t data_addr);
    uint8_t dec(uint16_t data_addr);
    uint8_t dex(uint16_t data_addr);
    uint8_t dey(uint16_t data_addr);
    uint8_t eor(uint16_t data_addr);
    uint8_t inc(uint16_t data_addr);
    uint8_t inx(uint16_t data_addr);
    uint8_t iny(uint16_t data_addr);
    uint8_t jmp(uint16_t data_addr);
    uint8_t jsr(uint16_t data_addr);
    uint8_t lsr(uint16_t data_addr);
    uint8_t nop(uint16_t data_addr);
    uint8_t ora(uint16_t data_addr);
    uint8_t pha(uint16_t data_addr);
    uint8_t php(uint16_t data_addr);
    uint8_t pla(uint16_t data_addr);
    uint8_t plp(uint16_t data_addr);
    uint8_t rol(uint16_t data_addr);
    uint8_t ror(uint16_t data_addr);
    uint8_t rti(uint16_t data_addr);
    uint8_t rts(uint16_t data_addr);
    uint8_t sbc(uint16_t data_addr);
    uint8_t sec(uint16_t data_addr);
    uint8_t sed(uint16_t data_addr);
    uint8_t sei(uint16_t data_addr);
    uint8_t sta(uint16_t data_addr);
    uint8_t stx(uint16_t data_addr);
    uint8_t sty(uint16_t data_addr);
    uint8_t tax(uint16_t data_addr);
    uint8_t tay(uint16_t data_addr);
    uint8_t tsx(uint16_t data_addr);
    uint8_t txa(uint16_t data_addr);
    uint8_t txs(uint16_t data_addr);
    uint8_t tya(uint16_t data_addr);

    // helpers
    uint8_t asl_acc(uint16_t data_addr);
    uint8_t asl_impl(uint8_t data);
    uint8_t lsr_acc(uint16_t data_addr);
    uint8_t rol_acc(uint16_t data_addr);
    uint8_t rol_impl(uint8_t data);
    uint8_t ror_acc(uint16_t data_addr);
    uint8_t ror_impl(uint8_t data);

    void displace_pc_from_data_addr(uint16_t data_addr);
    void adjust_zero_and_negative_flags(uint8_t data);

    void stack_push_byte(uint8_t data);
    void stack_push_word(uint16_t data);
    uint8_t stack_top_byte() const;
    uint16_t stack_top_word() const;
    uint8_t stack_pop_byte();
    uint16_t stack_pop_word();

    // TODO: Move this to a lookup table once we can
    using operation_fn_t  = uint8_t (CPU6502::*)(uint16_t);
    using addressing_fn_t = uint16_t (CPU6502::*)();
    struct Instruction
    {
        std::string name;
        operation_fn_t operation_fn;
        addressing_fn_t addressing_fn;
        uint8_t cycles;
    };
    std::unordered_map<uint8_t, Instruction> m_InstructionMap;

    bool verbose_log = false;
};
