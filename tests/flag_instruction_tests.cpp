#include <catch2/catch_test_macros.hpp>

#include "cpu.h"
#include "opcodes.h"

TEST_CASE("clc imp", "[clc],[cpu],[imp],[instruction]")
{
    CPU6502 cpu;
    cpu.registers.p.set_carry_bit();
    cpu.memory.write_byte(0, OPCODE_CLC_IMP);
    cpu.process_instruction();

    REQUIRE(!cpu.registers.p.carry_bit_set());
}
