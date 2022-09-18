#include <catch2/catch_test_macros.hpp>

#include "cpu.h"
#include "opcodes.h"

TEST_CASE("adc imm", "[adc],[cpu],[imm],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_ADC_IMM);

    SECTION("non zero, non negative, non overflow, non carry adc")
    {
        cpu.memory.write_byte(1, 42);
        cpu.registers.a = 43;
        cpu.process_instruction();

        REQUIRE(cpu.registers.a == 85);
        REQUIRE(cpu.registers.pc == 2);
        REQUIRE(!cpu.registers.p.carry_bit_set());
    }

    SECTION("zero flag set adc")
    {
        cpu.memory.write_byte(1, 0);
        cpu.registers.a = 0;
        cpu.process_instruction();

        REQUIRE(cpu.registers.a == 0);
        REQUIRE(cpu.registers.p.zero_flag_set());
    }

    SECTION("negative flag set adc")
    {
        const uint8_t imm_value = 0b10011001;
        cpu.memory.write_byte(1, imm_value);
        cpu.registers.a = 1;
        cpu.process_instruction();

        REQUIRE(cpu.registers.a == imm_value + 1);
        REQUIRE(cpu.registers.p.negative_flag_set());
    }

    SECTION("carry flag set with bit 7 overflow adc")
    {
        cpu.memory.write_byte(1, 0xF0);
        cpu.registers.a = 0x20;
        cpu.process_instruction();

        REQUIRE(cpu.registers.a == 0x10);
        REQUIRE(cpu.registers.p.carry_bit_set());
    }

    SECTION("overflow flag set with two positive numbers adc")
    {
        cpu.memory.write_byte(1, 64);
        cpu.registers.a = 64;
        cpu.process_instruction();

        REQUIRE(cpu.registers.a == 128);
        REQUIRE(cpu.registers.p.overflow_flag_set());
    }

    SECTION("overflow flag set with two negative numbers adc")
    {
        cpu.memory.write_byte(1, 130);
        cpu.registers.a = 130;
        cpu.process_instruction();

        REQUIRE(cpu.registers.p.overflow_flag_set());
        REQUIRE(cpu.registers.a == 4);
    }
}
