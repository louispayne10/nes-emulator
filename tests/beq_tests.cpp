#include <catch2/catch_test_macros.hpp>

#include "cpu.h"
#include "opcodes.h"

TEST_CASE("beq rel", "[beq],[cpu],[rel],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_BEQ_REL);

    SECTION("zero flag set beq")
    {
        cpu.memory.write_byte(1, 0x20);
        cpu.registers.p.set_zero_flag();
        cpu.process_instruction();

        REQUIRE(cpu.registers.pc == 0x22);
    }

    SECTION("zero flag not set beq")
    {
        cpu.memory.write_byte(1, 0x20);
        cpu.process_instruction();

        REQUIRE(cpu.registers.pc == 2);
    }
}
