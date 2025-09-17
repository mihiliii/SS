#pragma once

#include "Assembler/InstructionFormat.hpp"

#include <cstdint>
#include <vector>

#define BYTE 8
#define WORD 32

typedef uint32_t Register;
typedef std::vector<char> Memory;

class CPU {
public:

    const uint32_t PC_START = 0x40000000;
    const uint32_t SP_START = 0x00000000;

    Register& PC = _gpr[(int) REG::PC];
    Register& SP = _gpr[(int) REG::SP];

    Register& STATUS = _csr[(int) REG::STATUS];
    Register& CAUSE = _csr[(int) REG::CAUSE];
    Register& HANDLER = _csr[(int) REG::HANDLER];

    CPU(Memory& memory);

    void reset();

    void run();

    bool _running;

    Register _gpr[16];
    Register _csr[3];

    Memory& _memory;

private:

    instruction_format fetch_instruction();

    void execute_instruction(instruction_format instruction);

    void push(Register reg);

    Register pop();

    Register read_memory(uint32_t address);

    void write_memory(Register& reg, uint32_t address);
};
