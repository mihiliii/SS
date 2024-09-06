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
#define INSTRUCTION_FORMAT_REG_A(instruction)   ((instruction & 0x00F00000) >> 20)
#define INSTRUCTION_FORMAT_REG_B(instruction)   ((instruction & 0x000F0000) >> 16)
#define INSTRUCTION_FORMAT_REG_C(instruction)   ((instruction & 0x0000F000) >> 12)
#define INSTRUCTION_FORMAT_DISP(instruction)    (instruction & 0x00000FFF)

struct Operand;

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

enum struct OP_CODE {
    HALT = 0x0,
    INT = 0x1,
    CALL = 0x2,
    JMP = 0x3,
    XCHG = 0x4,
    AR = 0x5,
    LOG = 0x6,
    SHF = 0x7,
    ST = 0x8,
    LD = 0x9
};

enum struct MOD_CALL {
    CALL = 0x0,
    CALL_IND = 0x1
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
    POP = 0x3,
    GPR = 0x4,
    MEM_GPRB_GPRC_DISP2 = 0x6,
};

enum struct MOD_ST {
    MEM_GPRA_GPRB_DISP = 0x0,
    PUSH = 0x1,
    MEM_MEM_GPRA_GPRB_DISP = 0x2,
};

enum struct ADDR {
    IMMEDIATE,
    MEM_DIR,
    REG_DIR,
    REG_IND,
    REG_IND_OFF
};

class Instructions {
public:

    friend class Assembler;

    static void halt();

    static void interrupt();

    static void iret();

    static void call(uint32_t _value);

    static void call(std::string _symbol);

    static void arithmetic_logic_shift(OP_CODE _op, MOD_ALU _mod, uint8_t _gprS, uint8_t _gprD);

    static void jump(MOD_JMP _mod, uint8_t _gprA, uint8_t _gprB, uint8_t _gprC, uint32_t _value);

    static void jump(MOD_JMP _mod, uint8_t _gprA, uint8_t _gprB, uint8_t _gprC, std::string _symbol);

    static void push(uint8_t _gpr);

    static void pop(uint8_t _gpr);

    static void load(ADDR _addr, uint8_t _gprA, uint8_t _gprB, uint32_t _value);

    static void load(ADDR _addr, uint8_t _gprA, uint8_t _gprB, std::string _symbol);

    static void csr_load(uint8_t _csr, uint8_t _gpr);

    static void store(ADDR _addr, uint8_t _gprA, uint8_t _gprB, uint8_t _gprC, uint32_t _value);

    static void store(ADDR _addr, uint8_t _gprA, uint8_t _gprB, uint8_t _gprC, std::string _symbol);

    static void csr_store(uint8_t _csr, uint8_t _gpr);

    static void exchange(uint8_t _gprA, uint8_t _gprB);

private:

    static std::unordered_map<std::string, std::function<void()>> instruction_map;
};
