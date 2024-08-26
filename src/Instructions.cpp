#include "../inc/Instructions.hpp"

#include <cstdint>
#include <string>
#include <unordered_map>

#include "../inc/Assembler.hpp"

#define CREATE_INSTRUCTION(OP_CODE, MOD, RegA, RegB, RegC, disp)                                           \
    ((((OP_CODE) & 0xF) << 28) | (((MOD) & 0xF) << 24) | (((RegA) & 0xF) << 20) | (((RegB) & 0xF) << 16) | \
     (((RegC) & 0xF) << 12) | ((disp) & 0xFFF))

typedef uint32_t instruction_format;

void Instructions::halt() {
    instruction_format instruction = CREATE_INSTRUCTION((uint8_t) OP_CODE::HALT, 0, 0, 0, 0, 0);

    Assembler::current_section->appendContent(instruction);
}

void Instructions::arithmetic_logic_shift(OP_CODE _op, MOD_ALU _mod, uint8_t _gprS, uint8_t _gprD) {
    instruction_format instruction = CREATE_INSTRUCTION((uint8_t) _op, (uint8_t) _mod, _gprD, _gprD, _gprS, 0);

    Assembler::current_section->appendContent(instruction);
}

void Instructions::jump(MOD_JMP _mod, uint8_t _gprA, uint8_t _gprB, uint8_t _gprC, std::string _symbol) {
    if ((uint8_t) _mod < 0x8)
        _mod = (MOD_JMP) ((uint8_t) _mod + 0x8);

    Elf32_Sym* symbol_entry = Assembler::symbol_table->findSymbol(_symbol);

    if (symbol_entry == nullptr)
        symbol_entry = Assembler::symbol_table->addSymbol(_symbol, 0, false);

    Assembler::symbol_table->addSymbolReference(symbol_entry, Assembler::current_section->getLocationCounter(), true);
    instruction_format instruction = CREATE_INSTRUCTION((uint8_t) OP_CODE::JMP, (uint8_t) _mod, _gprA, _gprB, _gprC, 0);

    Assembler::current_section->appendContent(instruction);
}

void Instructions::jump(MOD_JMP _mod, uint8_t _gprA, uint8_t _gprB, uint8_t _gprC, uint32_t _disp) {
    if (_disp > 0xFFF)
        _mod = (MOD_JMP) ((uint8_t) _mod + 0x8);

    instruction_format instruction =
        CREATE_INSTRUCTION((uint8_t) OP_CODE::JMP, (uint8_t) _mod, _gprA, _gprB, _gprC, _disp);

    if ((uint8_t) _mod >= 0x8) {
        Assembler::current_section->getLiteralTable().addLiteralReference(
            _disp, Assembler::current_section->getLocationCounter()
        );
    }

    Assembler::current_section->appendContent(instruction);
}


void Instructions::load(LD_ADDR _addr, uint8_t _gprA, uint8_t _gprB, uint8_t _gprC, uint32_t _value) {
    instruction_format instruction;
    switch (_addr) {
        case LD_ADDR::LITERAL_GPR: {
            if (_value < 0xFFF) {
                instruction =
                    CREATE_INSTRUCTION((uint8_t) OP_CODE::LD, (uint8_t) MOD_LD::GPR_DISP, _gprA, 0, 0, _value);
            }
            else {
                Assembler::current_section->getLiteralTable().addLiteralReference(
                    _value, Assembler::current_section->getLocationCounter()
                );

                CREATE_INSTRUCTION((uint8_t) OP_CODE::LD, (uint8_t) MOD_LD::MEM_GPRB_GPRC_DISP, _gprA, 15, 0, _value);
            }

            break;
        }
        default:
        break;
    }

    Assembler::current_section->appendContent(instruction);
}

void Instructions::load(LD_ADDR _addr, uint8_t _gprA, uint8_t _gprB, uint8_t _gprC, std::string _symbol) {
    instruction_format instruction;

    Elf32_Sym* symbol_entry = Assembler::symbol_table->findSymbol(_symbol);

    if (symbol_entry == nullptr)
        symbol_entry = Assembler::symbol_table->addSymbol(_symbol, 0, false);

    Assembler::symbol_table->addSymbolReference(symbol_entry, Assembler::current_section->getLocationCounter(), true);
    instruction =
        CREATE_INSTRUCTION((uint8_t) LD_ADDR::SYMBOL_GPR, (uint8_t) MOD_LD::MEM_GPRB_GPRC_DISP, _gprA, 15, 0, 0);

    Assembler::current_section->appendContent(instruction);
}