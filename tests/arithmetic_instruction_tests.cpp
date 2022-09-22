#include <catch2/catch_test_macros.hpp>

#include "cpu.h"
#include "memory.h"
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

TEST_CASE("dec zp", "[dec],[cpu],[zp],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_DEC_ZP);

    SECTION("decrement basic")
    {
        cpu.memory.write_byte(1, 0x20);
        cpu.memory.write_byte(0x20, 42);
        cpu.process_instruction();

        REQUIRE(cpu.memory.read_byte(0x20) == 41);
    }
}

TEST_CASE("dex imp", "[dex],[cpu],[imp],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_DEX_IMP);

    SECTION("decrement x basic")
    {
        cpu.registers.x = 42;
        cpu.process_instruction();

        REQUIRE(cpu.registers.x == 41);
    }

    SECTION("decrement x wrap")
    {
        cpu.registers.x = 0;
        cpu.process_instruction();

        REQUIRE(cpu.registers.x == 0xFF);
    }
}

TEST_CASE("dey imp", "[dey],[cpu],[imp],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_DEY_IMP);

    SECTION("decrement y basic")
    {
        cpu.registers.y = 42;
        cpu.process_instruction();

        REQUIRE(cpu.registers.y == 41);
    }

    SECTION("decrement y wrap")
    {
        cpu.registers.y = 0;
        cpu.process_instruction();

        REQUIRE(cpu.registers.y == 0xFF);
    }
}

TEST_CASE("inc zp", "[inc],[cpu],[zp],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_INC_ZP);

    SECTION("non wrapping inc")
    {
        cpu.memory.write_byte(1, 0x20);
        cpu.memory.write_byte(0x20, 42);
        cpu.process_instruction();

        REQUIRE(cpu.memory.read_byte(0x20) == 43);
    }

    SECTION("wrapping inc")
    {
        cpu.memory.write_byte(1, 0x20);
        cpu.memory.write_byte(0x20, 0xFF);
        cpu.process_instruction();

        REQUIRE(cpu.memory.read_byte(0x20) == 0);
        REQUIRE(cpu.registers.p.zero_flag_set());
    }
}

TEST_CASE("inx imp", "[inx],[cpu],[imp],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_INX_IMP);

    SECTION("basic inx")
    {
        cpu.registers.x = 42;
        cpu.process_instruction();

        REQUIRE(cpu.registers.x == 43);
    }
}

TEST_CASE("iny imp", "[iny],[cpu],[imp],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_INY_IMP);

    SECTION("basic iny")
    {
        cpu.registers.y = 42;
        cpu.process_instruction();

        REQUIRE(cpu.registers.y == 43);
    }
}

TEST_CASE("lsr acc", "[lsr],[cpu],[acc],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_LSR_ACC);

    cpu.registers.a = 0b1010'1010;
    cpu.process_instruction();

    REQUIRE(cpu.registers.a == 0b0101'0101);
}

TEST_CASE("lsr zp", "[lsr],[cpu],[acc],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_LSR_ZP);

    cpu.memory.write_byte(1, 0x20);
    cpu.memory.write_byte(0x20, 0b0101'0101);
    cpu.process_instruction();

    REQUIRE(cpu.memory.read_byte(0x20) == 0b0010'1010);
    REQUIRE(cpu.registers.p.carry_bit_set());
}

TEST_CASE("rol acc", "[rol],[cpu],[acc],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_ROL_ACC);
    cpu.registers.p.set_carry_bit();
    cpu.registers.a = 0b1000'1100;
    cpu.process_instruction();

    REQUIRE(cpu.registers.a == 0b0001'1001);
    REQUIRE(cpu.registers.p.carry_bit_set());
}

TEST_CASE("rol zp", "[rol],[cpu],[zp],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_ROL_ZP);
    cpu.memory.write_byte(1, 0x20);
    cpu.memory.write_byte(0x20, 0b0101'0101);
    cpu.process_instruction();

    REQUIRE(cpu.memory.read_byte(0x20) == 0b1010'1010);
    REQUIRE(!cpu.registers.p.carry_bit_set());
}

TEST_CASE("ror acc", "[ror],[cpu],[acc],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_ROR_ACC);
    cpu.registers.p.set_carry_bit();
    cpu.registers.a = 0b1000'1101;
    cpu.process_instruction();

    REQUIRE(cpu.registers.a == 0b1100'0110);
    REQUIRE(cpu.registers.p.carry_bit_set());
}

TEST_CASE("ror zp", "[ror],[cpu],[zp],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_ROR_ZP);
    cpu.memory.write_byte(1, 0x20);
    cpu.memory.write_byte(0x20, 0b1010'1010);
    cpu.process_instruction();

    REQUIRE(cpu.memory.read_byte(0x20) == 0b0101'0101);
    REQUIRE(!cpu.registers.p.carry_bit_set());
}

TEST_CASE("sbc imm", "[sbc],[cpu],[imm],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_SBC_IMM);

    SECTION("basic sbc")
    {
        cpu.registers.a = 100;
        cpu.memory.write_byte(1, 50);
        cpu.process_instruction();

        // 150 - 50 - 255 (mod 256) = 101
        REQUIRE(cpu.registers.a == 51);
        REQUIRE(!cpu.registers.p.overflow_flag_set());
    }

    SECTION("sbc with carry flag")
    {
        cpu.registers.a = 150;
        cpu.memory.write_byte(1, 50);
        cpu.registers.p.set_carry_bit();
        cpu.process_instruction();

        // 150 - 50 - 254 = 102
        REQUIRE(cpu.registers.a == 102);
    }
}
