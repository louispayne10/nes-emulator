#include <catch2/catch_test_macros.hpp>

#include "cpu.h"
#include "opcodes.h"

TEST_CASE("cmp imm", "[cmp],[cpu],[imm],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_CMP_IMM);
    cpu.registers.a = 0x80;

    SECTION("greater than cmp")
    {
        cpu.memory.write_byte(1, 0x50);
        cpu.process_instruction();

        REQUIRE(cpu.registers.p.carry_bit_set());
        REQUIRE(!cpu.registers.p.zero_flag_set());
    }

    SECTION("equal to cmp")
    {
        cpu.memory.write_byte(1, 0x80);
        cpu.process_instruction();

        REQUIRE(cpu.registers.p.carry_bit_set());
        REQUIRE(cpu.registers.p.zero_flag_set());
    }

    SECTION("less than cmp")
    {
        cpu.memory.write_byte(1, 0x90);
        cpu.process_instruction();

        REQUIRE(!cpu.registers.p.carry_bit_set());
        REQUIRE(!cpu.registers.p.zero_flag_set());
    }
}

TEST_CASE("add imm", "[add],[cpu],[imm],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_AND_IMM);

    SECTION("non zero, non negative and")
    {
        cpu.memory.write_byte(1, 0b1100'1100);
        cpu.registers.a = 0b1110'0110;
        cpu.process_instruction();

        REQUIRE(cpu.registers.a == 0b1100'0100);
    }
}

TEST_CASE("bit zp", "[bit],[cpu],[zp],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_BIT_ZP);

    SECTION("zero flag set")
    {
        cpu.registers.p.set_negative_flag();
        cpu.registers.p.set_overflow_bit();
        cpu.memory.write_byte(1, 0x20);
        cpu.memory.write_byte(0x20, 0b0001'0001);
        cpu.registers.a = 0b0001'0000;
        cpu.process_instruction();

        REQUIRE(!cpu.registers.p.zero_flag_set());
        REQUIRE(!cpu.registers.p.overflow_flag_set());
        REQUIRE(!cpu.registers.p.negative_flag_set());
    }

    SECTION("zero flag cleared")
    {
        cpu.registers.p.set_zero_flag();
        cpu.memory.write_byte(1, 0x20);
        cpu.memory.write_byte(0x20, 0b1111'0000);
        cpu.registers.a = 0b0000'1111;
        cpu.process_instruction();

        REQUIRE(cpu.registers.p.zero_flag_set());
        REQUIRE(cpu.registers.p.overflow_flag_set());
        REQUIRE(cpu.registers.p.negative_flag_set());
    }
}

TEST_CASE("cpx imm", "[cpx],[cpu],[imm],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_CPX_IMM);
    cpu.registers.x = 0x80;

    SECTION("greater than cpx")
    {
        cpu.memory.write_byte(1, 0x50);
        cpu.process_instruction();

        REQUIRE(cpu.registers.p.carry_bit_set());
        REQUIRE(!cpu.registers.p.zero_flag_set());
    }

    SECTION("equal to cpx")
    {
        cpu.memory.write_byte(1, 0x80);
        cpu.process_instruction();

        REQUIRE(cpu.registers.p.carry_bit_set());
        REQUIRE(cpu.registers.p.zero_flag_set());
    }

    SECTION("less than cpx")
    {
        cpu.memory.write_byte(1, 0x90);
        cpu.process_instruction();

        REQUIRE(!cpu.registers.p.carry_bit_set());
        REQUIRE(!cpu.registers.p.zero_flag_set());
    }
}
