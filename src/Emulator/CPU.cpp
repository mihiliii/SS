#include "Emulator/CPU.hpp"
#include "Assembler/InstructionFormat.hpp"
#include "Emulator/Emulator.hpp"

#include <cstdint>
#include <cstring>
#include <iostream>

void CPU::push(Register reg)
{
    SP -= WORD / BYTE;
    std::memcpy(&_memory[SP], &reg, sizeof(instruction_format));
}

Register CPU::pop()
{
    Register reg;
    std::memcpy(&reg, &_memory[SP], sizeof(instruction_format));
    SP += WORD / BYTE;
    return reg;
}

CPU::CPU(Memory& memory) : _memory(memory)
{
    reset();
}

instruction_format CPU::fetch_instruction()
{
    instruction_format instruction;
    std::memcpy(&instruction, &_memory[PC], sizeof(instruction_format));
    PC += WORD / BYTE;
    return instruction;
}

void CPU::execute_instruction(instruction_format instruction)
{
    OC oc = if_get_oc(instruction);
    MOD mod = if_get_mod(instruction);
    REG regA = if_get_reg_a(instruction);
    REG regB = if_get_reg_b(instruction);
    REG regC = if_get_reg_c(instruction);
    int disp = if_get_disp(instruction);

    Register& reg_a = _gpr[(int) regA];
    Register& reg_b = _gpr[(int) regB];
    Register& reg_c = _gpr[(int) regC];

    if (disp & 0x800) {
        disp |= 0xFFFFF000;
    }

    switch (oc) {
    case OC::HALT:
        _running = false;
        break;
    case OC::INT:
        push(STATUS);
        push(PC);
        CAUSE = 4;
        STATUS = STATUS & (~0x1);
        PC = HANDLER;
        break;
    case OC::CALL:
        if (mod == MOD::CALL) {
            push(PC);
            PC = reg_a + reg_b + disp;
        }
        else if (mod == MOD::CALL_IND) {
            push(PC);
            PC = read_memory(reg_a + reg_b + disp);
        }
        break;
    case OC::JMP:
        if (mod == MOD::JMP) {
            PC = reg_a + disp;
        }
        else if (mod == MOD::BEQ && reg_c == reg_b) {
            PC = reg_a + disp;
        }
        else if (mod == MOD::BNE && reg_c != reg_b) {
            PC = reg_a + disp;
        }
        else if (mod == MOD::BGT && reg_c > reg_b) {
            PC = reg_a + disp;
        }
        else if (mod == MOD::JMP_IND) {
            PC = read_memory(reg_a + disp);
        }
        else if (mod == MOD::BEQ_IND && reg_c == reg_b) {
            PC = read_memory(reg_a + disp);
        }
        else if (mod == MOD::BNE_IND && reg_c != reg_b) {
            PC = read_memory(reg_a + disp);
        }
        else if (mod == MOD::BGT_IND && reg_c > reg_b) {
            PC = read_memory(reg_a + disp);
        }
        break;
    case OC::XCHG: {
        Register temp = reg_a;
        reg_a = reg_b;
        reg_b = temp;
        break;
    }
    case OC::AR:
        if (mod == MOD::AR_ADD) {
            reg_a = reg_b + reg_c;
        }
        else if (mod == MOD::AR_SUB) {
            reg_a = reg_b - reg_c;
        }
        else if (mod == MOD::AR_MUL) {
            reg_a = reg_b * reg_c;
        }
        else if (mod == MOD::AR_DIV) {
            reg_a = reg_b / reg_c;
        }
        break;
    case OC::LOG:
        if (mod == MOD::LOG_NOT) {
            reg_a = ~reg_b;
        }
        else if (mod == MOD::LOG_AND) {
            reg_a = reg_b & reg_c;
        }
        else if (mod == MOD::LOG_OR) {
            reg_a = reg_b | reg_c;
        }
        else if (mod == MOD::LOG_XOR) {
            reg_a = reg_b ^ reg_c;
        }
        break;
    case OC::SHF:
        if (mod == MOD::SHF_SHL) {
            reg_a = reg_b << reg_c;
        }
        else if (mod == MOD::SHF_SHR) {
            reg_a = reg_b >> reg_c;
        }
        break;
    case OC::LD:
        if (mod == MOD::LD_GPR_CSR) {
            reg_a = _csr[(int) regB];
        }
        else if (mod == MOD::LD_GPR_GPR_DSP) {
            reg_a = reg_b + disp;
        }
        else if (mod == MOD::LD_GPR_REGIND_DSP) {
            reg_a = read_memory(reg_b + reg_c + disp);
        }
        else if (mod == MOD::LD_GPR_REGIND_INC) {
            reg_a = read_memory(reg_b);
            reg_b = reg_b + disp;
        }
        else if (mod == MOD::LD_CSR_GPR) {
            _csr[(int) regA] = reg_b;
        }
        else if (mod == MOD::LD_CSR_OR) {
            _csr[(int) regA] = _csr[(int) regB] | disp;
        }
        else if (mod == MOD::LD_CSR_REGIND_DSP) {
            _csr[(int) regA] = read_memory(reg_b + reg_c + disp);
        }
        else if (mod == MOD::LD_CSR_REGIND_INC) {
            _csr[(int) regA] = read_memory(reg_b);
            reg_b = reg_b + disp;
        }
        break;
    case OC::ST:
        if (mod == MOD::ST_REGIND) {
            write_memory(reg_c, reg_a + reg_b + disp);
        }
        else if (mod == MOD::ST_INC_REGIND) {
            reg_a = reg_a + disp;
            write_memory(reg_c, reg_a);
        }
        else if (mod == MOD::ST_MEMIND_REGIND) {
            Register address;
            address = read_memory(reg_a + reg_b + disp);
            write_memory(reg_c, (uint32_t) address);
        }
        break;
    default:
        std::cout << "Invalid instruction." << std::endl;
        _running = false;
        break;
    }

    // R0 should always be zero
    _gpr[0] = 0;
}

Register CPU::read_memory(uint32_t address)
{
    Register return_value;
    std::memcpy(&return_value, &_memory[address], sizeof(instruction_format));
    return return_value;
}

void CPU::write_memory(Register& reg, uint32_t address)
{
    std::memcpy(&_memory[address], &reg, sizeof(instruction_format));
}

void CPU::reset()
{
    for (size_t i = 0; i < 16; i++) {
        _gpr[i] = 0;
    }
    for (size_t i = 0; i < 3; i++) {
        _csr[i] = 0;
    }
    PC = PC_START;
    SP = SP_START;
}

void CPU::run()
{
    _running = true;
    while (_running) {
        instruction_format instruction = fetch_instruction();
        execute_instruction(instruction);
    }
}
