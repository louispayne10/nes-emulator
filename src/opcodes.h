#pragma once

#include <cstdint>

enum OPCODE {
    OPCODE_BRK_IMP  = 0x00,
    OPCODE_ORA_INDX = 0x01,
    OPCODE_ORA_ZP   = 0x05,
    OPCODE_ASL_ZP   = 0x06,
    OPCODE_ORA_IMM  = 0x09,
    OPCODE_ASL_ACC  = 0x0A,
    OPCODE_ORA_ABS  = 0x0D,
    OPCODE_ASL_ABS  = 0x0E,
    OPCODE_BPL_REL  = 0x10,
    OPCODE_ORA_INDY = 0x11,
    OPCODE_ORA_ZPX  = 0x15,
    OPCODE_ASL_ZPX  = 0x16,
    OPCODE_CLC_IMP  = 0x18,
    OPCODE_ORA_ABSY = 0x19,
    OPCODE_ORA_ABSX = 0x1D,
    OPCODE_ASL_ABSX = 0x1E,
    OPCODE_JSR_ABS  = 0x20,
    OPCODE_AND_INDX = 0x21,
    OPCODE_BIT_ZP   = 0x24,
    OPCODE_AND_ZP   = 0x25,
    OPCODE_AND_IMM  = 0x29,
    OPCODE_BIT_ABS  = 0x2C,
    OPCODE_AND_ABS  = 0x2D,
    OPCODE_BMI_REL  = 0x30,
    OPCODE_AND_INDY = 0x31,
    OPCODE_AND_ZPX  = 0x35,
    OPCODE_AND_ABSX = 0x3D,
    OPCODE_AND_ABSY = 0x39,
    OPCODE_EOR_INDX = 0x41,
    OPCODE_EOR_ZP   = 0x45,
    OPCODE_LSR_ZP   = 0x46,
    OPCODE_EOR_IMM  = 0x49,
    OPCODE_LSR_ACC  = 0x4A,
    OPCODE_JMP_ABS  = 0x4C,
    OPCODE_EOR_ABS  = 0x4D,
    OPCODE_LSR_ABS  = 0x4E,
    OPCODE_BVC_REL  = 0x50,
    OPCODE_EOR_INDY = 0x51,
    OPCODE_EOR_ZPX  = 0x55,
    OPCODE_LSR_ZPX  = 0x56,
    OPCODE_CLI_IMP  = 0x58,
    OPCODE_EOR_ABSY = 0x59,
    OPCODE_EOR_ABSX = 0x5D,
    OPCODE_LSR_ABSX = 0x5E,
    OPCODE_ADC_INDX = 0x61,
    OPCODE_ADC_ZP   = 0x65,
    OPCODE_ADC_IMM  = 0x69,
    OPCODE_JMP_IND  = 0x6C,
    OPCODE_ADC_ABS  = 0x6D,
    OPCODE_BVS_REL  = 0x70,
    OPCODE_ADC_INDY = 0x71,
    OPCODE_ADC_ZPX  = 0x75,
    OPCODE_ADC_ABSX = 0x7D,
    OPCODE_ADC_ABSY = 0x79,
    OPCODE_DEY_IMP  = 0x88,
    OPCODE_BCC_REL  = 0x90,
    OPCODE_LDY_IMM  = 0xA0,
    OPCODE_LDA_INDX = 0xA1,
    OPCODE_LDX_IMM  = 0xA2,
    OPCODE_LDY_ZP   = 0xA4,
    OPCODE_LDA_ZP   = 0xA5,
    OPCODE_LDX_ZP   = 0xA6,
    OPCODE_LDA_IMM  = 0xA9,
    OPCODE_LDY_ABS  = 0xAC,
    OPCODE_LDA_ABS  = 0xAD,
    OPCODE_LDX_ABS  = 0xAE,
    OPCODE_BCS_REL  = 0xB0,
    OPCODE_LDA_INDY = 0xB1,
    OPCODE_LDY_ZPX  = 0xB4,
    OPCODE_LDA_ZPX  = 0xB5,
    OPCODE_LDX_ZPY  = 0xB6,
    OPCODE_CLV_IMP  = 0xB8,
    OPCODE_LDA_ABSY = 0xB9,
    OPCODE_LDY_ABSX = 0xBC,
    OPCODE_LDA_ABSX = 0xBD,
    OPCODE_LDX_ABSY = 0xBE,
    OPCODE_CPY_IMM  = 0xC0,
    OPCODE_CMP_INDX = 0xC1,
    OPCODE_CPY_ZP   = 0xC4,
    OPCODE_CMP_ZP   = 0xC5,
    OPCODE_DEC_ZP   = 0xC6,
    OPCODE_INY_IMP  = 0xC8,
    OPCODE_CMP_IMM  = 0xC9,
    OPCODE_DEX_IMP  = 0xCA,
    OPCODE_CPY_ABS  = 0xCC,
    OPCODE_CMP_ABS  = 0xCD,
    OPCODE_DEC_ABS  = 0xCE,
    OPCODE_BNE_REL  = 0xD0,
    OPCODE_CMP_INDY = 0xD1,
    OPCODE_CMP_ZPX  = 0xD5,
    OPCODE_DEC_ZPX  = 0xD6,
    OPCODE_CLD_IMP  = 0xD8,
    OPCODE_CMP_ABSY = 0xD9,
    OPCODE_CMP_ABSX = 0xDD,
    OPCODE_DEC_ABSX = 0xDE,
    OPCODE_CPX_IMM  = 0xE0,
    OPCODE_CPX_ZP   = 0xE4,
    OPCODE_INC_ZP   = 0xE6,
    OPCODE_INX_IMP  = 0xE8,
    OPCODE_NOP_IMP  = 0xEA,
    OPCODE_CPX_ABS  = 0xEC,
    OPCODE_INC_ABS  = 0xEE,
    OPCODE_BEQ_REL  = 0xF0,
    OPCODE_INC_ZPX  = 0xF6,
    OPCODE_INC_ABSX = 0xFE
};
