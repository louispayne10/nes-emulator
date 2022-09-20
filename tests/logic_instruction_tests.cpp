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
