#include <catch2/catch_test_macros.hpp>

#include "cpu.h"
#include "opcodes.h"

TEST_CASE("tax imp", "[tax],[cpu],[imp],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_TAX_IMP);
    cpu.registers.a = 42;
    cpu.process_instruction();

    REQUIRE(cpu.registers.x == 42);
}
