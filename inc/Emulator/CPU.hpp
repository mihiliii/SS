#pragma once

#include <cstdint>
#include <vector>

typedef uint32_t Register;
typedef uint32_t instruction_t;

#define GPR_PC   GPR[15]
#define GPR_SP   GPR[14]
#define GPR_ZERO GPR[0]

#define CSR_STATUS CSR[0]
#define CSR_HANDLE CSR[1]
#define CSR_CAUSE  CSR[2]

#define BYTE 8
#define WORD 32

typedef std::vector<char> Memory;

class CPU {
public:

    CPU(Memory& _memory) : memory(_memory) { reset(); }

    Register& getGPR(uint8_t _index) { return GPR[_index]; };
    Register& getCSR(uint8_t _index) { return CSR[_index]; };
    Register& getPC() { return GPR_PC; };
    Register& getSP() { return GPR_SP; };
    Register& getZERO() { return GPR_ZERO; };

    instruction_t fetchInstruction();
    void executeInstruction(instruction_t _instruction);

    void readMemory(Register& _register, uint32_t _address);
    void writeMemory(uint32_t _address, Register& _register);

    void printCurrentInstruction(instruction_t _instruction);

    void reset();

    void run();

    bool running;

    Register GPR[16];
    Register CSR[3];

    Memory& memory;
};
