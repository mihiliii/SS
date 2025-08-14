#pragma once

#include "../Elf32/Elf32.hpp"
#include <cstdint>

constexpr uint32_t SP_INC = 4;
constexpr uint32_t SP_DEC = -4;
constexpr uint32_t MAX_DISP = 0xFFF;

typedef Elf32_Word instruction_format;

constexpr uint32_t IF_MASK_OC = 0xF0000000;    // OC flags mask
constexpr uint32_t IF_MASK_MOD = 0x0F000000;   // OC modifier mask
constexpr uint32_t IF_MASK_REG_A = 0x00F00000; // Register A mask
constexpr uint32_t IF_MASK_REG_B = 0x000F0000; // Register B mask
constexpr uint32_t IF_MASK_REG_C = 0x0000F000; // Register C mask
constexpr uint32_t IF_MASK_DISP = 0x00000FFF;  // Displacement mask

constexpr uint8_t IF_SHIFT_OC = 28;    // OC modifier shift
constexpr uint8_t IF_SHIFT_MOD = 24;   // OC modifier shift
constexpr uint8_t IF_SHIFT_REG_A = 20; // Register A shift
constexpr uint8_t IF_SHIFT_REG_B = 16; // Register B shift
constexpr uint8_t IF_SHIFT_REG_C = 12; // Register C shift
constexpr uint8_t IF_SHIFT_DISP = 0;   // Displacement shift

enum struct REG {
    STATUS = 0x0,
    HANDLER = 0x1,
    CAUSE = 0x2,
    R0 = 0x0,
    R1 = 0x1,
    R2 = 0x2,
    R3 = 0x3,
    R4 = 0x4,
    R5 = 0x5,
    R6 = 0x6,
    R7 = 0x7,
    R8 = 0x8,
    R9 = 0x9,
    R10 = 0xA,
    R11 = 0xB,
    R12 = 0xC,
    R13 = 0xD,
    R14 = 0xE,
    R15 = 0xF,
    ZERO = 0x0,
    SP = 0xE,
    PC = 0xF
};

enum struct OC {
    HALT = 0x0,
    INT = 0x1,
    CALL = 0x2,
    JMP = 0x3,
    XCHG = 0x4,
    AR = 0x5,
    LOG = 0x6,
    SHF = 0x7,
    ST = 0x8,
    LD = 0x9,
};

enum struct MOD {
    HALT = 0x00,              // stops execution
    INT = 0x10,               // push status; push pc; cause<=4; status<=status&~(0x1); pc<=handle;
    CALL = 0x20,              // push pc; pc<=gpr[A]+gpr[B]+D;
    CALL_IND = 0x21,          // push pc; pc<=mem32[gpr[A]+gpr[B]+D];
    JMP = 0x30,               // pc<=gpr[A]+D;
    BEQ = 0x31,               // if (gpr[B] == gpr[C]) pc<=gpr[A]+D;
    BNE = 0x32,               // if (gpr[B] != gpr[C]) pc<=gpr[A]+D;
    BGT = 0x33,               // if (gpr[B] > gpr[C]) pc<=gpr[A]+D;
    JMP_IND = 0x38,           // pc<=mem32[gpr[A]+D];
    BEQ_IND = 0x39,           // if (gpr[B] == gpr[C]) pc<=mem32[gpr[A]+D];
    BNE_IND = 0x3A,           // if (gpr[B] != gpr[C]) pc<=mem32[gpr[A]+D];
    BGT_IND = 0x3B,           // if (gpr[B] > gpr[C]) pc<=mem32[gpr[A]+D];
    XCHG = 0x40,              // temp<=gpr[B]; gpr[B]<=gpr[C]; gpr[C]<=temp;
    AR_ADD = 0x50,            // gpr[A]<=gpr[B] + gpr[C];
    AR_SUB = 0x51,            // gpr[A]<=gpr[B] - gpr[C];
    AR_MUL = 0x52,            // gpr[A]<=gpr[B] * gpr[C];
    AR_DIV = 0x53,            // gpr[A]<=gpr[B] / gpr[C];
    LOG_NOT = 0x60,           // gpr[A]<=~gpr[B];
    LOG_AND = 0x61,           // gpr[A]<=gpr[B] & gpr[C];
    LOG_OR = 0x62,            // gpr[A]<=gpr[B] | gpr[C];
    LOG_XOR = 0x63,           // gpr[A]<=gpr[B] ^ gpr[C];
    SHF_SHL = 0x70,           // gpr[A]<=gpr[B] << gpr[C];
    SHF_SHR = 0x71,           // gpr[A]<=gpr[B] >> gpr[C];
    ST_REGIND = 0x80,         // mem32[gpr[A]+gpr[B]+D]<=gpr[C];
    ST_INC_REGIND = 0x81,     // gpr[A]<=gpr[A]+D; mem32[gpr[A]]<=gpr[C];
    ST_MEMIND_REGIND = 0x82,  // mem32[mem32[gpr[A]+gpr[B]+D]]<=gpr[C];
    LD_GPR_CSR = 0x90,        // gpr[A]<=csr[B];
    LD_GPR_GPR_DSP = 0x91,    // gpr[A]<=gpr[B]+D;
    LD_GPR_REGIND_DSP = 0x92, // gpr[A]<=mem32[gpr[B]+gpr[C]+D];
    LD_GPR_REGIND_INC = 0x93, // gpr[A]<=mem32[gpr[B]+gpr[C]+D]; gpr[B]<=gpr[B]+D;
    LD_CSR_GPR = 0x94,        // csr[A]<=gpr[B];
    LD_CSR_OR = 0x95,         // csr[A]<=csr[B]|D;
    LD_CSR_REGIND_DSP = 0x96, // csr[A]<=mem32[gpr[B]+gpr[C]+D];
    LD_CSR_REGIND_INC = 0x97  // csr[A]<=mem32[gpr[B]]; gpr[B]<=gpr[B]+D;
};

enum struct IF_ADDR {
    IMMEDIATE,  // immediate addressing
    MEM_DIR,    // memory direct addressing
    MEM_IND,    // memory indirect addressing
    REG_DIR,    // register direct addressing
    REG_IND,    // register indirect addressing
    REG_IND_OFF // register indirect with offset addressing
};

inline instruction_format if_create(OC oc, MOD mod, REG gpr_a, REG gpr_b, REG gpr_c, uint32_t disp)
{
    return ((instruction_format) oc << IF_SHIFT_OC) | ((instruction_format) mod << IF_SHIFT_MOD) |
           ((instruction_format) gpr_a << IF_SHIFT_REG_A) |
           ((instruction_format) gpr_b << IF_SHIFT_REG_B) |
           ((instruction_format) gpr_c << IF_SHIFT_REG_C) |
           ((instruction_format) disp & IF_MASK_DISP);
}

inline OC if_get_oc(instruction_format instruction)
{
    return (OC) ((instruction & IF_MASK_OC) >> IF_SHIFT_OC);
}

inline void if_set_oc(instruction_format& instruction, OC oc)
{
    instruction = (instruction & ~IF_MASK_OC) | ((instruction_format) oc << IF_SHIFT_OC);
}

inline MOD if_get_mod(instruction_format instruction)
{
    return (MOD) ((instruction & IF_MASK_MOD) >> IF_SHIFT_MOD);
}

inline void if_set_mod(instruction_format& instruction, MOD mod)
{
    instruction = (instruction & ~IF_MASK_MOD) | ((instruction_format) mod << IF_SHIFT_MOD);
}

inline REG if_get_reg_a(instruction_format instruction)
{
    return (REG) ((instruction & IF_MASK_REG_A) >> IF_SHIFT_REG_A);
}

inline void if_set_reg_a(instruction_format& instruction, REG reg_a)
{
    instruction = (instruction & ~IF_MASK_REG_A) | ((instruction_format) reg_a << IF_SHIFT_REG_A);
}

inline REG if_get_reg_b(instruction_format instruction)
{
    return (REG) ((instruction & IF_MASK_REG_B) >> IF_SHIFT_REG_B);
}

inline void if_set_reg_b(instruction_format& instruction, REG reg_b)
{
    instruction = (instruction & ~IF_MASK_REG_B) | ((instruction_format) reg_b << IF_SHIFT_REG_B);
}

inline REG if_get_reg_c(instruction_format instruction)
{
    return (REG) ((instruction & IF_MASK_REG_C) >> IF_SHIFT_REG_C);
}

inline void if_set_reg_c(instruction_format& instruction, REG reg_c)
{
    instruction = (instruction & ~IF_MASK_REG_C) | ((instruction_format) reg_c << IF_SHIFT_REG_C);
}

inline Elf32_Half if_get_disp(instruction_format instruction)
{
    return (Elf32_Half) instruction & IF_MASK_DISP;
}

inline void if_set_disp(instruction_format& instruction, Elf32_Half disp)
{
    instruction = (instruction & ~IF_MASK_DISP) | ((instruction_format) disp & IF_MASK_DISP);
}
