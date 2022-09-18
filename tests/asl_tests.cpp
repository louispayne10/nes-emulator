#include <catch2/catch_test_macros.hpp>

#include "cpu.h"
#include "opcodes.h"

TEST_CASE("asl acc", "[asl],[cpu],[acc],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_ASL_ACC);

    SECTION("non zero, non negative, no carry asl")
    {
        cpu.registers.a = 0b0001'0001;
        cpu.process_instruction();

        REQUIRE(cpu.registers.a == 0b0010'0010);
        REQUIRE(cpu.registers.pc == 1);
        REQUIRE(!cpu.registers.p.carry_bit_set());
    }

    SECTION("non zero, non negative, carry set asl")
    {
        cpu.registers.a = 0b1000'1000;
        cpu.process_instruction();

        REQUIRE(cpu.registers.a == 0b0001'0000);
        REQUIRE(cpu.registers.p.carry_bit_set());
    }
}

TEST_CASE("asl zp", "[asl],[cpu],[zp],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_ASL_ZP);

    SECTION("non zero, non negative, no carry asl")
    {
        cpu.memory.write_byte(1, 0x20);
        cpu.memory.write_byte(0x20, 0b0001'0001);
        cpu.process_instruction();

        REQUIRE(cpu.memory.read_byte(0x20) == 0b0010'0010);
        REQUIRE(!cpu.registers.p.carry_bit_set());
    }
}

TEST_CASE("asl abs", "[asl],[cpu],[abs],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_ASL_ABS);

    SECTION("non zero, non negative, carry asl")
    {
        cpu.memory.write_word(1, 0xB0);
        cpu.memory.write_byte(0xB0, 0b1000'1000);
        cpu.process_instruction();

        REQUIRE(cpu.memory.read_byte(0xB0) == 0b0001'0000);
        REQUIRE(cpu.registers.p.carry_bit_set());
    }
}
