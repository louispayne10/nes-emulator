#pragma once

#include <cstdint>

enum OPCODE {
    OPCODE_LDA_ZP   = 0xA5,
    OPCODE_LDA_IMM  = 0xA9,
    OPCODE_LDA_ABS  = 0xAD,
    OPCODE_LDA_ZPX  = 0xB5,
    OPCODE_LDA_ABSY = 0xB9,
    OPCODE_LDA_ABSX = 0xBD
};
