#include "../inc/Instructions.hpp"

#include <cstdint>
#include <string>
#include <unordered_map>

#include "../inc/Assembler.hpp"

#define CREATE_INSTRUCTION(OP_CODE, MOD, RegA, RegB, RegC, disp)                                           \
    ((((OP_CODE) & 0xF) << 28) | (((MOD) & 0xF) << 24) | (((RegA) & 0xF) << 20) | (((RegB) & 0xF) << 16) | \
     (((RegC) & 0xF) << 12) | ((disp) & 0xFFF))

typedef uint32_t instruction_format;

void Instructions::haltIns() {
    instruction_format instruction = CREATE_INSTRUCTION((uint8_t) OP_CODE::HALT, 0, 0, 0, 0, 0);

    Assembler::current_section->appendContent(instruction);
}

void Instructions::arithmeticIns(MOD_ALU _mod, uint8_t _gprS, uint8_t _gprD) {
    instruction_format instruction = CREATE_INSTRUCTION((uint8_t) OP_CODE::ALU, (uint8_t) _mod, _gprD, _gprD, _gprS, 0);

    Assembler::current_section->appendContent(instruction);
}

void Instructions::logicIns(MOD_LOG _mod, uint8_t _gprS, uint8_t _gprD) {
    instruction_format instruction = CREATE_INSTRUCTION((uint8_t) OP_CODE::ALU, (uint8_t) _mod, _gprD, _gprD, _gprS, 0);

    Assembler::current_section->appendContent(instruction);
}

void Instructions::shiftIns(MOD_SHF _mod, uint8_t _gprS, uint8_t _gprD) {
    instruction_format instruction = CREATE_INSTRUCTION((uint8_t) OP_CODE::ALU, (uint8_t) _mod, _gprD, _gprD, _gprS, 0);

    Assembler::current_section->appendContent(instruction);
}

void Instructions::jumpIns(OP_CODE _oc, uint8_t _gprA, uint8_t _gprB, uint8_t _gprC, std::string _symbol) {
    Elf32_Sym* symbol_entry = Assembler::symbol_table->findSymbol(_symbol);
    if (symbol_entry == nullptr) {
        Assembler::symbol_table->addSymbol(_symbol, 0, false);
        Elf32_Sym* symbol_entry = Assembler::symbol_table->findSymbol(_symbol);
        Assembler::symbol_table->addSymbolReference(
            symbol_entry, Assembler::current_section->getLocationCounter(), true
        );
    }
    else {
        Assembler::symbol_table->addSymbolReference(
            symbol_entry, Assembler::current_section->getLocationCounter(), true
        );
    }

    instruction_format instruction = CREATE_INSTRUCTION((uint8_t) _oc, (uint8_t) MOD_JMP::JMP_IND, _gprA, 0, 0, 0);

    Assembler::current_section->appendContent(instruction);
}

void Instructions::jumpIns(OP_CODE _oc, uint8_t _gprA, uint8_t _gprB, uint8_t _gprC, uint32_t _disp) {
    MOD_JMP mod;
    if (_disp > 0xFFF)
        mod = MOD_JMP::JMP_IND;
    else
        mod = MOD_JMP::JMP;

    switch (mod) {
        case MOD_JMP::JMP: {
            instruction_format instruction = CREATE_INSTRUCTION((uint8_t) _oc, (uint8_t) mod, _gprA, 0, 0, _disp);

            Assembler::current_section->appendContent(instruction);
            break;
        }
        case MOD_JMP::JMP_IND: {
            instruction_format instruction = CREATE_INSTRUCTION((uint8_t) _oc, (uint8_t) mod, _gprA, 0, 0, 0);

            Assembler::current_section->addLiteralReference(_disp, Assembler::current_section->getLocationCounter());
            Assembler::current_section->appendContent(instruction);
            break;
        }
        default:
            break;
    }
}
