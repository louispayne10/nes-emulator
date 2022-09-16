#include <catch2/catch_test_macros.hpp>

#include "cpu.h"
#include "opcodes.h"

TEST_CASE("ldx imm", "[ldx],[cpu],[imm],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_LDX_IMM);
    StatusRegister old_status_reg = cpu.registers.p;

    SECTION("non zero, non negative load")
    {
        cpu.memory.write_byte(1, 42);
        cpu.process_instruction();

        REQUIRE(cpu.registers.x == 42);
        REQUIRE(cpu.registers.p == old_status_reg);
    }

    SECTION("zero load")
    {
        cpu.memory.write_byte(1, 0);
        cpu.process_instruction();

        REQUIRE(cpu.registers.x == 0);
        REQUIRE(cpu.registers.p.zero_flag_set());
    }

    SECTION("negative load")
    {
        const uint8_t byte_to_load = 0b10011001;
        cpu.memory.write_byte(1, byte_to_load);
        cpu.process_instruction();

        REQUIRE(cpu.registers.x == byte_to_load);
        REQUIRE(cpu.registers.p.negative_flag_set());
    }
}

TEST_CASE("ldx zpy", "[ldx],[cpu],[zpy],[instruction]")
{
    CPU6502 cpu;
    cpu.memory.write_byte(0, OPCODE_LDX_ZPY);

    SECTION("non zero, non negative load")
    {
        cpu.memory.write_byte(1, 0x20);
        cpu.registers.y = 0x15;
        cpu.memory.write_byte(0x35, 42);
        cpu.process_instruction();

        REQUIRE(cpu.registers.x == 42);
    }
}
