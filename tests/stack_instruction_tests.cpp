#include <catch2/catch_test_macros.hpp>

#include "cpu.h"
#include "opcodes.h"

TEST_CASE("pha imp", "[pha],[cpu],[imp],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_PHA_IMP);
    cpu.registers.a = 0x42;
    cpu.process_instruction();

    REQUIRE(cpu.stack_top() == 0x42);
}

TEST_CASE("php imp", "[php],[cpu],[imp],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_PHP_IMP);
    cpu.registers.p.set_zero_flag();
    cpu.registers.p.set_overflow_bit();
    cpu.process_instruction();

    StatusRegister reg = StatusRegister{ (StatusRegFlag)cpu.stack_top() };
    REQUIRE(reg.zero_flag_set());
    REQUIRE(reg.overflow_flag_set());
}
