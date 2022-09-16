#pragma once

#include <cstdint>

enum OPCODE {
    OPCODE_LDA_INDX = 0xA1,
    OPCODE_LDX_IMM  = 0xA2,
    OPCODE_LDA_ZP   = 0xA5,
    OPCODE_LDX_ZP   = 0xA6,
    OPCODE_LDA_IMM  = 0xA9,
    OPCODE_LDA_ABS  = 0xAD,
    OPCODE_LDX_ABS  = 0xAE,
    OPCODE_LDA_INDY = 0xB1,
    OPCODE_LDA_ZPX  = 0xB5,
    OPCODE_LDX_ZPY  = 0xB6,
    OPCODE_LDA_ABSY = 0xB9,
    OPCODE_LDA_ABSX = 0xBD,
    OPCODE_LDX_ABSY = 0xBE
};
