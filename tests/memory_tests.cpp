#include <catch2/catch_test_macros.hpp>

#include "memory.h"

TEST_CASE("mirror r/w", "[memory],[RAM]")
{
    Memory memory;
    memory.write_byte(10, 42);

    REQUIRE(memory.read_byte(10) == 42);
    REQUIRE(memory.read_byte(10 + 0x800) == 42);
    REQUIRE(memory.read_byte(10 + 0x800 * 2) == 42);
    REQUIRE(memory.read_byte(10 + 0x800 * 3) == 42);
}
