#include <catch2/catch_test_macros.hpp>

#include "cpu.h"
#include "opcodes.h"

TEST_CASE("bne rel", "[bne],[cpu],[rel],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_BNE_REL);

    SECTION("zero flag set bne")
    {
        cpu.memory.write_byte(1, 0x20);
        cpu.registers.p.set_zero_flag();
        cpu.process_instruction();

        REQUIRE(cpu.registers.pc == 2);
    }

    SECTION("zero flag not set bne")
    {
        cpu.memory.write_byte(1, 0x20);
        cpu.process_instruction();

        REQUIRE(cpu.registers.pc == 0x22);
    }
}

TEST_CASE("beq rel", "[beq],[cpu],[rel],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_BEQ_REL);

    SECTION("zero flag set beq")
    {
        cpu.memory.write_byte(1, 0x20);
        cpu.registers.p.set_zero_flag();
        cpu.process_instruction();

        REQUIRE(cpu.registers.pc == 0x22);
    }

    SECTION("zero flag not set beq")
    {
        cpu.memory.write_byte(1, 0x20);
        cpu.process_instruction();

        REQUIRE(cpu.registers.pc == 2);
    }
}

TEST_CASE("bcc rel", "[bcc],[cpu],[rel],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_BCC_REL);

    SECTION("carry clear positive branch bcc")
    {
        cpu.memory.write_byte(1, 0x20);
        cpu.registers.p.clear_carry_flag();
        cpu.process_instruction();

        REQUIRE(cpu.registers.pc == 0x22);
    }

    SECTION("carry clear negative branch bcc")
    {
        cpu.memory.write_byte(10, OPCODE_BCC_REL);
        cpu.registers.pc = 10;
        cpu.memory.write_byte(11, 0b1111'1100); // -4
        cpu.registers.p.clear_carry_flag();
        cpu.process_instruction();

        REQUIRE(cpu.registers.pc == 8);
    }

    SECTION("carry set bcc")
    {
        cpu.memory.write_byte(1, 1);
        cpu.registers.p.set_carry_bit();
        cpu.process_instruction();

        REQUIRE(cpu.registers.pc == 2);
    }
}

TEST_CASE("bcs rel", "[bcs],[cpu],[rel],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_BCS_REL);

    SECTION("carry set positive branch bcs")
    {
        cpu.memory.write_byte(1, 0x20);
        cpu.registers.p.set_carry_bit();
        cpu.process_instruction();

        REQUIRE(cpu.registers.pc == 0x22);
    }

    SECTION("carry set negative branch bcs")
    {
        cpu.memory.write_byte(10, OPCODE_BCS_REL);
        cpu.registers.pc = 10;
        cpu.memory.write_byte(11, 0b1111'1100); // -4
        cpu.registers.p.set_carry_bit();
        cpu.process_instruction();

        REQUIRE(cpu.registers.pc == 8);
    }

    SECTION("carry clear bcs")
    {
        cpu.memory.write_byte(1, 1);
        cpu.registers.p.clear_carry_flag();
        cpu.process_instruction();

        REQUIRE(cpu.registers.pc == 2);
    }
}

TEST_CASE("bmi rel", "[bmi],[cpu],[rel],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_BMI_REL);

    cpu.memory.write_byte(1, 20);
    cpu.registers.p.set_negative_flag();
    cpu.process_instruction();

    REQUIRE(cpu.registers.pc == 22);
}

TEST_CASE("bpl rel", "[bpl],[cpu],[rel],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_BPL_REL);

    cpu.memory.write_byte(1, 20);
    cpu.registers.p.clear_negative_flag();
    cpu.process_instruction();

    REQUIRE(cpu.registers.pc == 22);
}

TEST_CASE("bvc rel", "[bvc],[cpu],[rel],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_BVC_REL);

    cpu.memory.write_byte(1, 20);
    cpu.registers.p.clear_overflow_flag();
    cpu.process_instruction();

    REQUIRE(cpu.registers.pc == 22);
}

TEST_CASE("bvs rel", "[bvs],[cpu],[rel],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_BVS_REL);

    cpu.memory.write_byte(1, 20);
    cpu.registers.p.set_overflow_bit();
    cpu.process_instruction();

    REQUIRE(cpu.registers.pc == 22);
}

TEST_CASE("jmp abs", "[jmp],[cpu],[abs],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_JMP_ABS);

    cpu.memory.write_word(1, 0x220);
    cpu.memory.write_byte(0x220, 0x40);
    cpu.process_instruction();

    REQUIRE(cpu.registers.pc == 0x40);
}

TEST_CASE("jmp ind", "[jmp],[cpu],[ind],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_JMP_IND);

    cpu.memory.write_byte(1, 0x34);
    cpu.memory.write_byte(2, 0x12);
    cpu.memory.write_byte(0x1234, 0x50);
    cpu.memory.write_byte(0x1235, 0x12);
    cpu.memory.write_byte(0x1250, 42);
    cpu.process_instruction();

    REQUIRE(cpu.registers.pc == 42);
}
