#include "Assembler/InstructionFormat.hpp"

#include <iostream>

std::string gpr_to_s(REG reg)
{
    switch (reg) {
    case REG::R0:
        return "R0";
    case REG::R1:
        return "R1";
    case REG::R2:
        return "R2";
    case REG::R3:
        return "R3";
    case REG::R4:
        return "R4";
    case REG::R5:
        return "R5";
    case REG::R6:
        return "R6";
    case REG::R7:
        return "R7";
    case REG::R8:
        return "R8";
    case REG::R9:
        return "R9";
    case REG::R10:
        return "R10";
    case REG::R11:
        return "R11";
    case REG::R12:
        return "R12";
    case REG::SP:
        return "SP";
    case REG::PC:
        return "PC";
    default:
        return "UNKNOWN";
    }
}

std::string csr_to_s(REG reg)
{
    switch (reg) {
    case REG::STATUS:
        return "STATUS";
    case REG::HANDLER:
        return "HANDLER";
    case REG::CAUSE:
        return "CAUSE";
    default:
        return "UNKNOWN";
    }
}

void print_function(OC oc, MOD mod, REG regA, REG regB, REG regC, int disp)
{

    switch (oc) {
    case OC::HALT:
        std::cout << "HALT" << std::endl;
        break;
    case OC::INT:
        std::cout << "INT" << std::endl;
        break;
    case OC::CALL:
        if (mod == MOD::CALL) {
            std::cout << "CALL " << gpr_to_s(regA) << ", " << gpr_to_s(regB) << ", " << disp
                      << std::endl;
        }
        else if (mod == MOD::CALL_IND) {
            std::cout << "CALL_IND " << gpr_to_s(regA) << ", " << gpr_to_s(regB) << ", " << disp
                      << std::endl;
        }
        break;
    case OC::JMP:
        if (mod == MOD::JMP) {
            std::cout << "JMP " << gpr_to_s(regA) << ", " << disp << std::endl;
        }
        else if (mod == MOD::BEQ) {
            std::cout << "BEQ " << gpr_to_s(regA) << ", " << gpr_to_s(regB) << ", "
                      << gpr_to_s(regC) << ", " << disp << std::endl;
        }
        else if (mod == MOD::BNE) {
            std::cout << "BNE " << gpr_to_s(regA) << ", " << gpr_to_s(regB) << ", "
                      << gpr_to_s(regC) << ", " << disp << std::endl;
        }
        else if (mod == MOD::BGT) {
            std::cout << "BGT " << gpr_to_s(regA) << ", " << gpr_to_s(regB) << ", "
                      << gpr_to_s(regC) << ", " << disp << std::endl;
        }
        else if (mod == MOD::JMP_IND) {
            std::cout << "JMP_IND " << gpr_to_s(regA) << ", " << disp << std::endl;
        }
        else if (mod == MOD::BEQ_IND) {
            std::cout << "BEQ_IND " << gpr_to_s(regA) << ", " << gpr_to_s(regB) << ", "
                      << gpr_to_s(regC) << ", " << disp << std::endl;
        }
        else if (mod == MOD::BNE_IND) {
            std::cout << "BNE_IND " << gpr_to_s(regA) << ", " << gpr_to_s(regB) << ", "
                      << gpr_to_s(regC) << ", " << disp << std::endl;
        }
        else if (mod == MOD::BGT_IND) {
            std::cout << "BGT_IND " << gpr_to_s(regA) << ", " << gpr_to_s(regB) << ", "
                      << gpr_to_s(regC) << ", " << disp << std::endl;
        }
        break;
    case OC::XCHG:
        std::cout << "XCHG " << gpr_to_s(regA) << ", " << gpr_to_s(regB) << std::endl;
        break;
    case OC::AR:
        if (mod == MOD::AR_ADD) {
            std::cout << "ADD " << gpr_to_s(regA) << ", " << gpr_to_s(regB) << ", "
                      << gpr_to_s(regC) << std::endl;
        }
        else if (mod == MOD::AR_SUB) {
            std::cout << "SUB " << gpr_to_s(regA) << ", " << gpr_to_s(regB) << ", "
                      << gpr_to_s(regC) << std::endl;
        }
        else if (mod == MOD::AR_MUL) {
            std::cout << "MUL " << gpr_to_s(regA) << ", " << gpr_to_s(regB) << ", "
                      << gpr_to_s(regC) << std::endl;
        }
        else if (mod == MOD::AR_DIV) {
            std::cout << "DIV " << gpr_to_s(regA) << ", " << gpr_to_s(regB) << ", "
                      << gpr_to_s(regC) << std::endl;
        }
        break;
    case OC::LOG:
        if (mod == MOD::LOG_NOT) {
            std::cout << "NOT " << gpr_to_s(regA) << ", " << gpr_to_s(regB) << std::endl;
        }
        else if (mod == MOD::LOG_AND) {
            std::cout << "AND " << gpr_to_s(regA) << ", " << gpr_to_s(regB) << ", "
                      << gpr_to_s(regC) << std::endl;
        }
        else if (mod == MOD::LOG_OR) {
            std::cout << "OR " << gpr_to_s(regA) << ", " << gpr_to_s(regB) << ", " << gpr_to_s(regC)
                      << std::endl;
        }
        else if (mod == MOD::LOG_XOR) {
            std::cout << "XOR " << gpr_to_s(regA) << ", " << gpr_to_s(regB) << ", "
                      << gpr_to_s(regC) << std::endl;
        }
        break;
    case OC::SHF:
        if (mod == MOD::SHF_SHL) {
            std::cout << "SHL " << gpr_to_s(regA) << ", " << gpr_to_s(regB) << ", "
                      << gpr_to_s(regC) << std::endl;
        }
        else if (mod == MOD::SHF_SHR) {
            std::cout << "SHR " << gpr_to_s(regA) << ", " << gpr_to_s(regB) << ", "
                      << gpr_to_s(regC) << std::endl;
        }
        break;
    case OC::LD:
        if (mod == MOD::LD_GPR_CSR) {
            std::cout << "LD_GPR_CSR " << gpr_to_s(regA) << ", " << csr_to_s(regB) << std::endl;
        }
        else if (mod == MOD::LD_GPR_GPR_DSP) {
            std::cout << "LD_GPR_GPR_DSP " << gpr_to_s(regA) << ", " << gpr_to_s(regB) << ", "
                      << disp << std::endl;
        }
        else if (mod == MOD::LD_GPR_REGIND_DSP) {
            std::cout << "LD_GPR_REGIND_DSP " << gpr_to_s(regA) << ", " << gpr_to_s(regB) << ", "
                      << gpr_to_s(regC) << ", " << disp << std::endl;
        }
        else if (mod == MOD::LD_GPR_REGIND_INC) {
            std::cout << "LD_GPR_REGIND_INC " << gpr_to_s(regA) << ", " << gpr_to_s(regB) << ", "
                      << gpr_to_s(regC) << ", " << disp << std::endl;
        }
        else if (mod == MOD::LD_CSR_GPR) {
            std::cout << "LD_CSR_GPR " << csr_to_s(regA) << ", " << gpr_to_s(regB) << std::endl;
        }
        else if (mod == MOD::LD_CSR_OR) {
            std::cout << "LD_CSR_OR " << csr_to_s(regA) << ", " << csr_to_s(regB) << ", " << disp
                      << std::endl;
        }
        else if (mod == MOD::LD_CSR_REGIND_DSP) {
            std::cout << "LD_CSR_REGIND_DSP " << csr_to_s(regA) << ", " << gpr_to_s(regB) << ", "
                      << gpr_to_s(regC) << ", " << disp << std::endl;
        }
        else if (mod == MOD::LD_CSR_REGIND_INC) {
            std::cout << "LD_CSR_REGIND_INC " << csr_to_s(regA) << ", " << gpr_to_s(regB) << ", "
                      << disp << std::endl;
        }
        break;
    case OC::ST:
        if (mod == MOD::ST_REGIND) {
            std::cout << "ST_REGIND " << gpr_to_s(regA) << ", " << gpr_to_s(regB) << ", "
                      << gpr_to_s(regC) << ", " << disp << std::endl;
        }
        else if (mod == MOD::ST_INC_REGIND) {
            std::cout << "ST_INC_REGIND " << gpr_to_s(regA) << ", " << gpr_to_s(regC) << ", "
                      << disp << std::endl;
        }
        else if (mod == MOD::ST_MEMIND_REGIND) {
            std::cout << "ST_MEMIND_REGIND " << gpr_to_s(regA) << ", " << gpr_to_s(regB) << ", "
                      << gpr_to_s(regC) << ", " << disp << std::endl;
        }
        break;
    default:
        std::cout << "Invalid instruction." << std::endl;
        break;
    }
}
