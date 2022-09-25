#include <catch2/catch_test_macros.hpp>

#include "cpu.h"
#include "opcodes.h"

TEST_CASE("pha imp", "[pha],[cpu],[imp],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_PHA_IMP);
    cpu.registers.a = 0x42;
    cpu.process_instruction();

    REQUIRE(cpu.stack_top_byte() == 0x42);
}

TEST_CASE("php imp", "[php],[cpu],[imp],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_PHP_IMP);
    cpu.registers.p.set_zero_flag();
    cpu.registers.p.set_overflow_bit();
    cpu.process_instruction();

    StatusRegister reg = StatusRegister{ (StatusRegFlag)cpu.stack_top_byte() };
    REQUIRE(reg.zero_flag_set());
    REQUIRE(reg.overflow_flag_set());
}

TEST_CASE("pla imp", "[pla],[cpu],[imp],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_PLA_IMP);
    cpu.stack_push_byte(42);
    cpu.process_instruction();

    REQUIRE(cpu.registers.a == 42);
}

TEST_CASE("plp imp", "[plp],[cpu],[imp],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_PLP_IMP);
    cpu.stack_push_byte((uint8_t)(StatusRegFlag::Carry | StatusRegFlag::Overflow));
    cpu.process_instruction();

    REQUIRE(cpu.registers.p.carry_bit_set());
    REQUIRE(cpu.registers.p.overflow_flag_set());
}
