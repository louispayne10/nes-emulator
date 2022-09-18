#include <catch2/catch_test_macros.hpp>

#include "cpu.h"
#include "opcodes.h"

TEST_CASE("bcs rel", "[bcs],[cpu],[rel],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_BCS_REL);

    SECTION("carry set positive branch bcs")
    {
        cpu.memory.write_byte(1, 0x20);
        cpu.registers.p.set_carry_bit();
        cpu.process_instruction();

        REQUIRE(cpu.registers.pc == 0x22);
    }

    SECTION("carry set negative branch bcs")
    {
        cpu.memory.write_byte(10, OPCODE_BCS_REL);
        cpu.registers.pc = 10;
        cpu.memory.write_byte(11, 0b1111'1100); // -4
        cpu.registers.p.set_carry_bit();
        cpu.process_instruction();

        REQUIRE(cpu.registers.pc == 8);
    }

    SECTION("carry clear bcs")
    {
        cpu.memory.write_byte(1, 1);
        cpu.registers.p.clear_carry_bit();
        cpu.process_instruction();

        REQUIRE(cpu.registers.pc == 2);
    }
}
