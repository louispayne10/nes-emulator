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
    cpu.process_instruction();

    REQUIRE(cpu.registers.pc == 0x220);
}

TEST_CASE("jmp ind", "[jmp],[cpu],[ind],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_JMP_IND);
    cpu.memory.write_word(1, 0x500);
    cpu.memory.write_word(0x500, 0x600);

    cpu.process_instruction();

    REQUIRE(cpu.registers.pc == 0x600);
}

TEST_CASE("jsr abs", "[jsr],[cpu],[ind],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_JSR_ABS);

    cpu.memory.write_word(1, 0x220);
    cpu.process_instruction();

    REQUIRE(cpu.registers.pc == 0x220);
    // jsr pushes the address (minus one) of the return point onto the stack
    // since jsr is a 3 byte instruction the return point should be 3
    // hence 2 should be pushed onto the stack
    REQUIRE(cpu.stack_top_word() == 2);
}

TEST_CASE("nop imp", "[nop],[cpu],[imp],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_NOP_IMP);

    cpu.process_instruction();

    REQUIRE(cpu.registers.pc == 1);
}

TEST_CASE("rts imp", "[rts],[cpu],[imp],[instruction]")
{
    CPU6502 cpu;
    cpu.registers.pc = 40;
    cpu.memory.write_byte(40, OPCODE_JSR_ABS);
    cpu.memory.write_word(41, 0xC400);
    cpu.memory.write_byte(0xC400, OPCODE_RTS_IMP);

    cpu.process_instruction();
    REQUIRE(cpu.registers.pc == 0xC400);
    REQUIRE(cpu.stack_top_word() == 42);

    cpu.process_instruction();
    REQUIRE(cpu.registers.pc == 43);
}
