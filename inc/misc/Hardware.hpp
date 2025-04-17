#pragma once

#include <cstdint>

typedef uint32_t instruction_t;

constexpr uint8_t WordSize = 4;
constexpr long MemSize = 0x100000000;
constexpr uint8_t SP_Inc = WordSize;
constexpr uint8_t SP_Dec = -WordSize;

enum struct CSR {
    STATUS = 0x0,
    HANDLER = 0x1,
    CAUSE = 0x2
};

enum struct GPR {
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

enum struct OpMode : uint8_t {
    HALT = 0x00,
    INT = 0x10,
    CALL = 0x20,                // push PC; PC = gpr[A] + gpr[B] + DISP;
    CALL_IND = 0x21,            // push PC; PC = mem32[gpr[A] + gpr[B] + DISP];
    JMP = 0x30,                 // PC = gpr[A];
    BEQ = 0x31,                 // if (gpr[B] == gpr[C]) PC = gpr[A] + DISP;
    BNE = 0x32,                 // if (gpr[B] != gpr[C]) PC = gpr[A] + DISP;
    BGT = 0x33,                 // if (gpr[B] > gpr[C]) PC = gpr[A] + DISP;
    JMP_IND = 0x38,             // PC = mem32[gpr[A]];
    BEQ_IND = 0x39,             // if (gpr[B] == gpr[C]) PC = mem32[gpr[A] + DISP];
    BNE_IND = 0x3A,             // if (gpr[B] != gpr[C]) PC = mem32[gpr[A] + DISP];
    BGT_IND = 0x3B,             // if (gpr[B] > gpr[C]) PC = mem32[gpr[A] + DISP];
    XCHG = 0x4,                 // gpr[A] = gpr[B]; gpr[B] = gpr[A];
    ADD = 0x50,                 // gpr[A] = gpr[B] + gpr[C];
    SUB = 0x51,                 // gpr[A] = gpr[B] - gpr[C];
    MUL = 0x52,                 // gpr[A] = gpr[B] * gpr[C];
    DIV = 0x53,                 // gpr[A] = gpr[B] / gpr[C];
    NOT = 0x60,                 // gpr[A] = ~gpr[B];
    AND = 0x61,                 // gpr[A] = gpr[B] & gpr[C];
    OR = 0x62,                  // gpr[A] = gpr[B] | gpr[C];
    XOR = 0x63,                 // gpr[A] = gpr[B] ^ gpr[C];
    SHL = 0x70,                 // gpr[A] = gpr[B] << 1;
    SHR = 0x71,                 // gpr[A] = gpr[B] >> 1;
    ST_REGIND_DISP = 0x80,      // mem32[gpr[A] + gpr[B] + DISP] = gpr[C];
    ST_PUSH = 0x81,             // gpr[A] += DISP; mem32[gpr[A]] = gpr[C];
    ST_MEM_REGIND_DISP = 0x82,  // mem32[mem32[gpr[A] + gpr[B] + DISP]] = gpr[C];
    LD_GPR_CSR = 0x90,          // gpr[A] = csr[B];
    LD_GPR_GPR_DISP = 0x91,     // gpr[A] = gpr[B] + DISP;
    LD_GPR_REGIND_DISP = 0x92,  // gpr[A] = mem32[gpr[B] + gpr[C] + DISP];
    LD_GPR_POP = 0x93,          // gpr[A] = mem32[gpr[B]]; gpr[B] += DISP;
    LD_CSR_GPR = 0x94,          // csr[A] = gpr[B];
    LD_CSR_REGIND_DISP = 0x96   // csr[A] = mem32[gpr[B] + gpr[C] + DISP];
};
