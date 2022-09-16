#include <catch2/catch_test_macros.hpp>

#include "cpu.h"
#include "opcodes.h"

TEST_CASE("lda imm", "[lda],[cpu],[imm],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_LDA_IMM);
    StatusRegister old_status_reg = cpu.registers.p;

    SECTION("non zero, non negative load")
    {
        cpu.memory.write_byte(1, 4);
        cpu.process_instruction();

        REQUIRE(cpu.registers.a == 4);
        REQUIRE(cpu.registers.p == old_status_reg);
        REQUIRE(cpu.registers.pc == 2);
    }

    SECTION("zero load")
    {
        cpu.memory.write_byte(1, 0);
        cpu.process_instruction();

        REQUIRE(cpu.registers.a == 0);
        REQUIRE(cpu.registers.p.zero_flag_set());
    }

    SECTION("negative load")
    {
        const uint8_t byte_to_load = 0b10011001;
        cpu.memory.write_byte(1, byte_to_load);
        cpu.process_instruction();

        REQUIRE(cpu.registers.a == byte_to_load);
        REQUIRE(cpu.registers.p.negative_flag_set());
    }
}

TEST_CASE("lda zp", "[lda],[cpu],[zp],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_LDA_ZP);
    const uint8_t data_offset = 0x20;
    cpu.memory.write_byte(1, data_offset);

    SECTION("non zero, non negative load")
    {
        cpu.memory.write_byte(data_offset, 59);
        cpu.process_instruction();

        REQUIRE(cpu.registers.pc == 2);
        REQUIRE(cpu.registers.a == 59);
    }

    SECTION("zero load")
    {
        cpu.memory.write_byte(data_offset, 0);
        cpu.process_instruction();

        REQUIRE(cpu.registers.pc == 2);
        REQUIRE(cpu.registers.a == 0);
        REQUIRE(cpu.registers.p.zero_flag_set());
    }

    SECTION("negative load")
    {
        const uint8_t byte_to_load = 0b10011001;
        cpu.memory.write_byte(data_offset, byte_to_load);
        cpu.process_instruction();

        REQUIRE(cpu.registers.pc == 2);
        REQUIRE(cpu.registers.a == byte_to_load);
        REQUIRE(cpu.registers.p.negative_flag_set());
    }
}

TEST_CASE("lda zpx", "[lda],[cpu],[zpx],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_LDA_ZPX);
    const uint8_t offset_from_instruction = 0x20;
    cpu.memory.write_byte(1, offset_from_instruction);

    SECTION("non zero, non negative, non wrapped load")
    {
        cpu.registers.x = 0x15;
        cpu.memory.write_byte(offset_from_instruction + 0x15, 42);
        cpu.process_instruction();

        REQUIRE(cpu.registers.pc == 2);
        REQUIRE(cpu.registers.a == 42);
    }

    SECTION("non zero, non negative, wrapped load")
    {
        cpu.registers.x              = 0xF0;
        const uint16_t addr_to_write = 0xF0 + offset_from_instruction - 0x100;
        cpu.memory.write_byte(addr_to_write, 42);
        cpu.process_instruction();

        REQUIRE(cpu.registers.pc == 2);
        REQUIRE(cpu.registers.a == 42);
    }
}

TEST_CASE("lda abs", "[lda],[cpu],[abs],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_LDA_ABS);

    SECTION("non zero, non negative, first page load")
    {
        // Here we write the 2 byte operand a byte at a time
        // this helps test that are endian code stuff works
        cpu.memory.write_byte(1, 0xA0);
        cpu.memory.write_byte(2, 0x00);
        cpu.memory.write_byte(0xA0, 42);
        cpu.process_instruction();

        REQUIRE(cpu.registers.pc == 3);
        REQUIRE(cpu.registers.a == 42);
    }

    SECTION("non zero, non negative, non first page load")
    {
        cpu.memory.write_byte(1, 0xF0);
        cpu.memory.write_byte(2, 0x01);
        cpu.memory.write_byte(0x1F0, 42);
        cpu.process_instruction();

        REQUIRE(cpu.registers.pc == 3);
        REQUIRE(cpu.registers.a == 42);
    }
}

TEST_CASE("lda absx", "[lda],[cpu],[absx],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_LDA_ABSX);

    SECTION("non zero, non negative, unwrapped load")
    {
        cpu.memory.write_word(1, 0xB0);
        cpu.registers.x = 0x10;
        cpu.memory.write_byte(0xC0, 42);
        cpu.process_instruction();

        REQUIRE(cpu.registers.pc == 3);
        REQUIRE(cpu.registers.a == 42);
    }

    SECTION("non zero, non negative, wrapped load")
    {
        // TODO: verify that this is the behaviour that is supposed to happen
        // I've assumed that if X + addr is greater than 0xFFFF that it wraps
        // around to 0 - this is the same for absy
        cpu.memory.write_word(1, 0xFFF0);
        cpu.registers.x = 0xFF;
        cpu.memory.write_byte(0xEF, 42);
        cpu.process_instruction();

        REQUIRE(cpu.registers.pc == 3);
        REQUIRE(cpu.registers.a == 42);
    }
}

TEST_CASE("lda absy", "[lda],[cpu],[absy],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_LDA_ABSY);

    SECTION("non zero, non negative, unwrapped load")
    {
        cpu.memory.write_word(1, 0xB0);
        cpu.registers.y = 0x10;
        cpu.memory.write_byte(0xC0, 42);
        cpu.process_instruction();

        REQUIRE(cpu.registers.pc == 3);
        REQUIRE(cpu.registers.a == 42);
    }

    SECTION("non zero, non negative, wrapped load")
    {
        cpu.memory.write_word(1, 0xFFF0);
        cpu.registers.y = 0xFF;
        cpu.memory.write_byte(0xEF, 42);
        cpu.process_instruction();

        REQUIRE(cpu.registers.pc == 3);
        REQUIRE(cpu.registers.a == 42);
    }
}

TEST_CASE("lda indx", "[lda],[cpu],[indx],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_LDA_INDX);
    cpu.memory.write_byte(1, 0xF0);

    SECTION("non zero, non negative, unwrapped load")
    {
        cpu.registers.x = 4;
        cpu.memory.write_byte(0xF4, 0x12);
        cpu.memory.write_byte(0xF5, 0x34);
        cpu.memory.write_byte(0x3412, 42);
        cpu.process_instruction();

        REQUIRE(cpu.registers.pc == 2);
        REQUIRE(cpu.registers.a == 42);
    }

    SECTION("non zero, non negative, wrapped load")
    {
        cpu.registers.x = 0x20;
        cpu.memory.write_byte(0x10, 0x12);
        cpu.memory.write_byte(0x11, 0x34);
        cpu.memory.write_byte(0x3412, 42);
        cpu.process_instruction();

        REQUIRE(cpu.registers.pc == 2);
        REQUIRE(cpu.registers.a == 42);
    }
}

TEST_CASE("lda indy", "[lda],[cpu],[indy],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_LDA_INDY);

    SECTION("non ero, non negative load")
    {
        cpu.memory.write_byte(1, 0x20);
        cpu.memory.write_byte(0x20, 0x12);
        cpu.memory.write_byte(0x21, 0x34);
        cpu.registers.y = 0x10;
        cpu.memory.write_byte(0x3412 + 0x10, 42);
        cpu.process_instruction();

        REQUIRE(cpu.registers.pc == 2);
        REQUIRE(cpu.registers.a == 42);
    }
}
