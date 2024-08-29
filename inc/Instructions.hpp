#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>

#define CREATE_INSTRUCTION(OP_CODE, MOD, RegA, RegB, RegC, disp)                                           \
    ((((OP_CODE) & 0xF) << 28) | (((MOD) & 0xF) << 24) | (((RegA) & 0xF) << 20) | (((RegB) & 0xF) << 16) | \
     (((RegC) & 0xF) << 12) | ((disp) & 0xFFF))

#define INSTRUCTION_FORMAT_OP_CODE(instruction) ((instruction & 0xF0000000) >> 28)
#define INSTRUCTION_FORMAT_MOD(instruction)     ((instruction & 0x0F000000) >> 24)
#define INSTRUCTION_FORMAT_REGA(instruction)    ((instruction & 0x00F00000) >> 20)
#define INSTRUCTION_FORMAT_REGB(instruction)    ((instruction & 0x000F0000) >> 16)
#define INSTRUCTION_FORMAT_REGC(instruction)    ((instruction & 0x0000F000) >> 12)
#define INSTRUCTION_FORMAT_DISP(instruction)    (instruction & 0x00000FFF)

struct operand;

enum struct OP_CODE {
    HALT = 0x0,
    JMP = 0x3,
    AR = 0x5,
    LOG = 0x6,
    SHF = 0x7,
    ST = 0x8,
    LD = 0x9
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
    NOT = 0x0,
    AND = 0x1,
    OR = 0x2,
    XOR = 0x3,
    SHL = 0x0,
    SHR = 0x1,
};

enum struct MOD_LD {
    CSR = 0x0,
    GPR_DISP = 0x1,
    MEM_GPRB_GPRC_DISP = 0x2,
};

enum struct LD_ADDR {
    LITERAL_GPR = 0x0,
    SYMBOL_GPR = 0x1,
};

class Instructions {
public:

    friend class Assembler;

    static void halt();

    static void arithmetic_logic_shift(OP_CODE _op, MOD_ALU _mod, uint8_t _gprS, uint8_t _gprD);

    static void jump(MOD_JMP _mod, uint8_t _gprA, uint8_t _gprB, uint8_t _gprC, uint32_t _value);

    static void jump(MOD_JMP _mod, uint8_t _gprA, uint8_t _gprB, uint8_t _gprC, std::string _symbol);

    static void load(LD_ADDR _addr, uint8_t _gprA, uint8_t _gprB, uint8_t _gprC, uint32_t _value);

    static void load(LD_ADDR _addr, uint8_t _gprA, uint8_t _gprB, uint8_t _gprC, std::string _symbol);

private:

    static std::unordered_map<std::string, std::function<void()>> instruction_map;
};
