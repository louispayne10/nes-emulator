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

TEST_CASE("tay imp", "[tay],[cpu],[imp],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_TAY_IMP);
    cpu.registers.a = 43;
    cpu.process_instruction();

    REQUIRE(cpu.registers.y == 43);
}

TEST_CASE("tsx imp", "[tsx],[cpu],[imp],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_TSX_IMP);
    cpu.stack_push(40);
    cpu.process_instruction();

    REQUIRE(cpu.registers.x == 0xFE);
}

TEST_CASE("txa imp", "[txa],[cpu],[imp],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_TXA_IMP);
    cpu.registers.x = 45;
    cpu.process_instruction();

    REQUIRE(cpu.registers.a == 45);
}

TEST_CASE("txs imp", "[txs],[cpu],[imp],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_TXS_IMP);
    cpu.registers.x = 0xF0;
    cpu.process_instruction();

    REQUIRE(cpu.registers.s == 0xF0);
}

TEST_CASE("tya imp", "[tya],[cpu],[imp],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_TYA_IMP);
    cpu.registers.y = 46;
    cpu.process_instruction();

    REQUIRE(cpu.registers.a == 46);
}
