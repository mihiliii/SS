#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>

#define OC_MASK   0x0000000F
#define MOD_MASK  0x000000F0
#define REGA_MASK 0x00000F00
#define REGB_MASK 0x0000F000
#define REGC_MASK 0x000F0000
#define DISP_MASK 0xFFF00000

enum struct OP_CODE {
    HALT = 0x0,
    JMP = 0x3,
    ALU = 0x5,
    LOG = 0x6,
    SHF = 0x7
};

enum struct MOD_JMP {
    JMP = 0x0,
    BEQ = 0x1,
    BNE = 0x2,
    BGT = 0x3,
    JMP_IND = 0x8,
    BEQ_IND = 0x9,
    BNE_IND = 0xA,
    BGT_IND = 0xB,
};

enum struct MOD_ALU {
    ADD = 0x0,
    SUB = 0x1,
    MUL = 0x2,
    DIV = 0x3,
};

enum struct MOD_LOG {
    NOT = 0x0,
    AND = 0x1,
    OR = 0x2,
    XOR = 0x3,
};

enum struct MOD_SHF {
    SHL = 0x0,
    SHR = 0x1,
};

class Instructions {
public:

    friend class Assembler;

    static void haltIns();

    static void arithmeticIns(MOD_ALU _mod, uint8_t _gprS, uint8_t _gprD);

    static void logicIns(MOD_LOG _mod, uint8_t _gprS, uint8_t _gprD);

    static void shiftIns(MOD_SHF _mod, uint8_t _gprS, uint8_t _gprD);

    static void jumpIns(MOD_JMP _mod, uint8_t _gprA, uint8_t _gprB, uint8_t _gprC, uint32_t _disp);

    static void jumpIns(MOD_JMP _mod, uint8_t _gprA, uint8_t _gprB, uint8_t _gprC, std::string _symbol);

private:

    static std::unordered_map<std::string, std::function<void()>> instruction_map;
};
