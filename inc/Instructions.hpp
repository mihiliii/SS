#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>

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
