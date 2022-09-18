#include <catch2/catch_test_macros.hpp>

#include "cpu.h"
#include "opcodes.h"

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
