#include "../../inc/Emulator/CPU.hpp"

#include "../../inc/Assembler/Instructions.hpp"

#define PUSH(_register) \
    GPR_SP -= 4;        \
    memory[GPR_SP] = _register
#define POP(_register)          \
    _register = memory[GPR_SP]; \
    GPR_SP += 4

void CPU::fetchInstruction() {
    
}

void CPU::executeInstruction(instruction_t _instruction) {
    uint8_t op_code = INSTRUCTION_FORMAT_OP_CODE(_instruction);
    uint8_t mod = INSTRUCTION_FORMAT_MOD(_instruction);
    uint8_t regA = INSTRUCTION_FORMAT_GPR_A(_instruction);
    uint8_t regB = INSTRUCTION_FORMAT_GPR_B(_instruction);
    uint8_t regC = INSTRUCTION_FORMAT_GPR_C(_instruction);
    uint16_t disp = INSTRUCTION_FORMAT_DISP(_instruction);

    switch (op_code) {
        case (int) OP_CODE::HALT:
            // Halt the CPU
            running = false;
            break;
        case (int) OP_CODE::INT:
            // Interrupt
            PUSH(CSR_STATUS);
            PUSH(GPR_PC);
            CSR_CAUSE = 4;
            CSR_STATUS = CSR_STATUS & (~0x1);
            GPR_PC = CSR_HANDLE;
            break;
        case (int) OP_CODE::CALL:
            // Call operations
            if (mod == (int) MOD_CALL::CALL) {
                PUSH(GPR_PC);
                GPR_PC = GPR[regA] + GPR[regB] + disp;
            } else if (mod == (int) MOD_CALL::CALL_IND) {
                POP(GPR_PC);
                GPR_PC = memory[GPR[regA] + GPR[regB] + disp];
            }
            break;
        case (int) OP_CODE::JMP:
            // Jump operations
            if (mod == (int) MOD_JMP::JMP)
                GPR_PC = GPR[regA] + disp;
            else if (mod == (int) MOD_JMP::BEQ && GPR[regC] == GPR[regB])
                GPR_PC = GPR[regA] + disp;
            else if (mod == (int) MOD_JMP::BNE && GPR[regC] != GPR[regB])
                GPR_PC = GPR[regA] + disp;
            else if (mod == (int) MOD_JMP::BGT && GPR[regC] > GPR[regB])
                GPR_PC = GPR[regA] + disp;
            else if (mod == (int) MOD_JMP::JMP_IND)
                GPR_PC = memory[GPR[regA] + disp];
            else if (mod == (int) MOD_JMP::BEQ_IND && GPR[regC] == GPR[regB])
                GPR_PC = memory[GPR[regA] + disp];
            else if (mod == (int) MOD_JMP::BNE_IND && GPR[regC] != GPR[regB])
                GPR_PC = memory[GPR[regA] + disp];
            else if (mod == (int) MOD_JMP::BGT_IND && GPR[regC] > GPR[regB])
                GPR_PC = memory[GPR[regA] + disp];
            break;
        case (int) OP_CODE::XCHG:
            // Exchange operations
            Register temp = GPR[regA];
            GPR[regA] = GPR[regB];
            GPR[regB] = temp;
            break;
        case (int) OP_CODE::AR:
            // Arithmetic operations
            if (mod == (int) MOD_ALU::ADD)
                GPR[regA] = GPR[regB] + GPR[regC];
            else if (mod == (int) MOD_ALU::SUB)
                GPR[regA] = GPR[regB] - GPR[regC];
            else if (mod == (int) MOD_ALU::MUL)
                GPR[regA] = GPR[regB] * GPR[regC];
            else if (mod == (int) MOD_ALU::DIV)
                GPR[regA] = GPR[regB] / GPR[regC];
            break;
        case (int) OP_CODE::LOG:
            // Logical operations
            if (mod == (int) MOD_ALU::NOT)
                GPR[regA] = ~GPR[regB];
            else if (mod == (int) MOD_ALU::AND)
                GPR[regA] = GPR[regB] & GPR[regC];
            else if (mod == (int) MOD_ALU::OR)
                GPR[regA] = GPR[regB] | GPR[regC];
            else if (mod == (int) MOD_ALU::XOR)
                GPR[regA] = GPR[regB] ^ GPR[regC];
            break;
        case (int) OP_CODE::SHF:
            // Shift operations
            if (mod == (int) MOD_ALU::SHL)
                GPR[regA] = GPR[regB] << GPR[regC];
            else if (mod == (int) MOD_ALU::SHR)
                GPR[regA] = GPR[regB] >> GPR[regC];
            break;
        case (int) OP_CODE::LD:
            // Load operations
            if (mod == (int) MOD_LD::CSR)
                GPR[regA] = CSR[regB];
            else if (mod == (int) MOD_LD::GPR_DISP)
                GPR[regA] = GPR[regB] + disp;
            else if (mod == (int) MOD_LD::MEM_GPRB_GPRC_DISP)
                GPR[regA] = memory[GPR[regB] + GPR[regC] + disp];
            else if (mod == (int) MOD_LD::POP) {
                GPR[regA] = memory[GPR[regB]];
                GPR[regB] = GPR[regB] + disp;
            } else if (mod == (int) MOD_LD::GPR)
                CSR[regA] = GPR[regB];
            else if (mod == (int) MOD_LD::MEM_GPRB_GPRC_DISP2)
                CSR[regA] = memory[GPR[regB] + GPR[regC] + disp];
            break;
        case (int) OP_CODE::ST:
            // Store operations
            if (mod == (int) MOD_ST::MEM_GPRA_GPRB_DISP)
                memory[GPR[regA] + GPR[regB] + disp] = GPR[regC];
            else if (mod == (int) MOD_ST::PUSH) {
                memory[GPR[regA]] = GPR[regB];
                GPR[regA] = GPR[regA] + disp;
            } else if (mod == (int) MOD_ST::MEM_MEM_GPRA_GPRB_DISP)
                memory[memory[GPR[regA] + GPR[regB] + disp]] = GPR[regC];
        default:
            break;
    }
}

void CPU::reset() {
    GPR_PC = 0x40000000;
    GPR_SP = 0x0;
}
