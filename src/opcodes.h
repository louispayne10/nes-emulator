#pragma once

#include <cstdint>

enum OPCODE {
    OPCODE_LDA_ZP  = 0xA5,
    OPCODE_LDA_IMM = 0xA9,
    OPCODE_LDA_ZPX = 0xB5
};
