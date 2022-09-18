#include <catch2/catch_test_macros.hpp>

#include "cpu.h"
#include "opcodes.h"

TEST_CASE("bcc rel", "[bcc],[cpu],[rel],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_BCC_REL);

    SECTION("carry clear positive branch bcc")
    {
        cpu.memory.write_byte(1, 0x20);
        cpu.registers.p.clear_carry_bit();
        cpu.process_instruction();

        REQUIRE(cpu.registers.pc == 0x22);
    }

    SECTION("carry clear negative branch bcc")
    {
        cpu.memory.write_byte(10, OPCODE_BCC_REL);
        cpu.registers.pc = 10;
        cpu.memory.write_byte(11, 0b1111'1100); // -4
        cpu.registers.p.clear_carry_bit();
        cpu.process_instruction();

        REQUIRE(cpu.registers.pc == 8);
    }

    SECTION("carry set bcc")
    {
        cpu.memory.write_byte(1, 1);
        cpu.registers.p.set_carry_bit();
        cpu.process_instruction();

        REQUIRE(cpu.registers.pc == 2);
    }
}
