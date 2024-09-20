#include "../../inc/Assembler/Instructions.hpp"

#include <cstdint>
#include <string>
#include <unordered_map>

#include "../../inc/Assembler/Assembler.hpp"
#include "../../inc/Assembler/ForwardReferenceTable.hpp"
#include "../../inc/Elf32File.hpp"
#include "../../inc/LiteralTable.hpp"

typedef uint32_t instruction_format;

extern uint32_t line;

void Instructions::halt() {
    Assembler::current_section->append(CREATE_INSTRUCTION((uint8_t) OP_CODE::HALT, 0, 0, 0, 0, 0));
}

void Instructions::interrupt() {
    Assembler::current_section->append(CREATE_INSTRUCTION((uint8_t) OP_CODE::INT, 0, 0, 0, 0, 0));
}

void Instructions::iret() {
    Assembler::current_section->append(CREATE_INSTRUCTION(
        (uint8_t) OP_CODE::LD, (uint8_t) MOD_LD::MEM_GPRB_GPRC_DISP2, (uint8_t) CSR::STATUS, (uint8_t) GPR::R14, 0, 4
    ));

    Assembler::current_section->append(
        CREATE_INSTRUCTION((uint8_t) OP_CODE::LD, (uint8_t) MOD_LD::POP, (uint8_t) GPR::R15, (uint8_t) GPR::R14, 0, 8)
    );
}

void Instructions::call(uint32_t _value) {
    instruction_format instruction;

    if (_value < 0xFFF) {
        instruction = CREATE_INSTRUCTION((uint8_t) OP_CODE::CALL, (uint8_t) MOD_CALL::CALL, 0, 0, 0, _value);
    }
    else {
        Assembler::current_section->getLiteralTable()->addLiteralReference(
            _value, Assembler::current_section->size()
        );

        instruction =
            CREATE_INSTRUCTION((uint8_t) OP_CODE::CALL, (uint8_t) MOD_CALL::CALL_IND, (uint8_t) GPR::PC, 0, 0, 0);
    }

    Assembler::current_section->append(instruction);
}

void Instructions::call(std::string _symbol) {
    Elf32_Sym* symbol_entry = Assembler::elf32_file->getSymbolTable().get(_symbol);

    if (symbol_entry == nullptr)
        symbol_entry = Assembler::elf32_file->getSymbolTable().add(_symbol, 0, false, Assembler::current_section->index());

    Assembler::forward_reference_table.add(symbol_entry, Assembler::current_section->size());
    Assembler::current_section->append(
        CREATE_INSTRUCTION((uint8_t) OP_CODE::CALL, (uint8_t) MOD_CALL::CALL_IND, (uint8_t) GPR::PC, 0, 0, 0)
    );
}

void Instructions::arithmetic_logic_shift(OP_CODE _op, MOD_ALU _mod, uint8_t _gprS, uint8_t _gprD) {
    Assembler::current_section->append(CREATE_INSTRUCTION((uint8_t) _op, (uint8_t) _mod, _gprD, _gprD, _gprS, 0)
    );
}

void Instructions::jump(MOD_JMP _mod, uint8_t _gprA, uint8_t _gprB, uint8_t _gprC, uint32_t _disp) {
    instruction_format instruction;

    if (_disp > 0xFFF) {
        _mod = (MOD_JMP) ((uint8_t) _mod + 0x8);
        Assembler::current_section->getLiteralTable()->addLiteralReference(
            _disp, Assembler::current_section->size()
        );
        instruction = CREATE_INSTRUCTION((uint8_t) OP_CODE::JMP, (uint8_t) _mod, (uint8_t) GPR::PC, _gprB, _gprC, 0);
    }
    else {
        instruction =
            CREATE_INSTRUCTION((uint8_t) OP_CODE::JMP, (uint8_t) _mod, (uint8_t) GPR::R0, _gprB, _gprC, _disp);
    }

    Assembler::current_section->append(instruction);
}

void Instructions::jump(MOD_JMP _mod, uint8_t _gprA, uint8_t _gprB, uint8_t _gprC, std::string _symbol) {
    if ((uint8_t) _mod < 0x8)
        _mod = (MOD_JMP) ((uint8_t) _mod + 0x8);

    Elf32_Sym* symbol_entry = Assembler::elf32_file->getSymbolTable().get(_symbol);

    if (symbol_entry == nullptr)
        symbol_entry = Assembler::elf32_file->getSymbolTable().add(_symbol, 0, false, Assembler::current_section->index());

    Assembler::forward_reference_table.add(symbol_entry, Assembler::current_section->size());
    instruction_format instruction =
        CREATE_INSTRUCTION((uint8_t) OP_CODE::JMP, (uint8_t) _mod, (uint32_t) GPR::PC, _gprB, _gprC, 0);

    Assembler::current_section->append(instruction);
}

void Instructions::push(uint8_t _gpr) {
    Assembler::current_section->append(
        CREATE_INSTRUCTION((uint8_t) OP_CODE::ST, (uint8_t) MOD_ST::PUSH, (uint32_t) GPR::SP, 0, _gpr, -4)
    );
}

void Instructions::pop(uint8_t _gpr) {
    Assembler::current_section->append(
        CREATE_INSTRUCTION((uint8_t) OP_CODE::LD, (uint8_t) MOD_LD::POP, _gpr, (uint32_t) GPR::SP, 0, 4)
    );
}

void Instructions::load(ADDR _addr, uint8_t _gprA, uint8_t _gprB, uint32_t _value) {
    instruction_format instruction;

    switch (_addr) {
        case ADDR::IMMEDIATE: {
            if (_value < 0xFFF) {
                instruction =
                    CREATE_INSTRUCTION((uint8_t) OP_CODE::LD, (uint8_t) MOD_LD::GPR_DISP, _gprA, 0, 0, _value);
            }
            else {
                Assembler::current_section->getLiteralTable()->addLiteralReference(
                    _value, Assembler::current_section->size()
                );

                instruction = CREATE_INSTRUCTION(
                    (uint8_t) OP_CODE::LD, (uint8_t) MOD_LD::MEM_GPRB_GPRC_DISP, _gprA, (uint32_t) GPR::PC, 0, 0
                );
            }
            break;
        }
        case ADDR::MEM_DIR: {
            if (_value < 0xFFF) {
                instruction = CREATE_INSTRUCTION(
                    (uint8_t) OP_CODE::LD, (uint8_t) MOD_LD::MEM_GPRB_GPRC_DISP, _gprA, 0, 0, _value
                );
            }
            else {
                Assembler::current_section->getLiteralTable()->addLiteralReference(
                    _value, Assembler::current_section->size()
                );

                instruction = CREATE_INSTRUCTION(
                    (uint8_t) OP_CODE::LD, (uint8_t) MOD_LD::MEM_GPRB_GPRC_DISP, _gprA, (uint32_t) GPR::PC, 0, 0
                );

                Assembler::current_section->append(instruction);

                instruction =
                    CREATE_INSTRUCTION((uint8_t) OP_CODE::LD, (uint8_t) MOD_LD::MEM_GPRB_GPRC_DISP, _gprA, _gprA, 0, 0);
            }
            break;
        }
        case ADDR::REG_DIR: {
            instruction = CREATE_INSTRUCTION((uint8_t) OP_CODE::LD, (uint8_t) MOD_LD::GPR_DISP, _gprA, _gprB, 0, 0);
            break;
        }
        case ADDR::REG_IND: {
            instruction =
                CREATE_INSTRUCTION((uint8_t) OP_CODE::LD, (uint8_t) MOD_LD::MEM_GPRB_GPRC_DISP, _gprA, _gprB, 0, 0);
            break;
        }
        case ADDR::REG_IND_OFF: {
            if (_value > 0xFFF) {
                std::cout << std::dec << "Error at line " << line << ": ";
                std::cout << "offset is too large" << std::endl;
                exit(-1);
            }
            instruction = CREATE_INSTRUCTION(
                (uint8_t) OP_CODE::LD, (uint8_t) MOD_LD::MEM_GPRB_GPRC_DISP, _gprA, _gprB, 0, _value
            );
        }
        default:
            break;
    }

    Assembler::current_section->append(instruction);
}

void Instructions::load(ADDR _addr, uint8_t _gprA, uint8_t _gprB, std::string _symbol) {
    instruction_format instruction;

    Elf32_Sym* symbol_entry = Assembler::elf32_file->getSymbolTable().get(_symbol);

    if (symbol_entry == nullptr)
        symbol_entry = Assembler::elf32_file->getSymbolTable().add(_symbol, 0, false, Assembler::current_section->index());

    switch (_addr) {
        case ADDR::IMMEDIATE: {
            Assembler::forward_reference_table.add(symbol_entry, Assembler::current_section->size());
            instruction = CREATE_INSTRUCTION(
                (uint8_t) OP_CODE::LD, (uint8_t) MOD_LD::MEM_GPRB_GPRC_DISP, _gprA, (uint32_t) GPR::PC, 0, 0
            );
            break;
        }
        case ADDR::MEM_DIR: {
            Assembler::forward_reference_table.add(symbol_entry, Assembler::current_section->size());
            instruction = CREATE_INSTRUCTION(
                (uint8_t) OP_CODE::LD, (uint8_t) MOD_LD::MEM_GPRB_GPRC_DISP, _gprA, (uint32_t) GPR::PC, 0, 0
            );

            Assembler::current_section->append(instruction);

            instruction =
                CREATE_INSTRUCTION((uint8_t) OP_CODE::LD, (uint8_t) MOD_LD::MEM_GPRB_GPRC_DISP, _gprA, _gprA, 0, 0);
            break;
        }
        default:
            break;
    }

    Assembler::current_section->append(instruction);
}

void Instructions::csr_load(uint8_t _csr, uint8_t _gpr) {
    Assembler::current_section->append(
        CREATE_INSTRUCTION((uint8_t) OP_CODE::LD, (uint8_t) MOD_LD::GPR, _csr, _gpr, 0, 0)
    );
}

void Instructions::store(ADDR _addr, uint8_t _gprA, uint8_t _gprB, uint8_t _gprC, uint32_t _value) {
    instruction_format instruction;

    switch (_addr) {
        case ADDR::IMMEDIATE: {
            std::cout << std::dec << "Error at line " << line << ": ";
            std::cout << "immediate addressing usage in store instruction" << std::endl;
            exit(-1);
            break;
        }
        case ADDR::MEM_DIR: {
            if (_value < 0xFFF) {
                instruction = CREATE_INSTRUCTION(
                    (uint8_t) OP_CODE::ST, (uint8_t) MOD_ST::MEM_GPRA_GPRB_DISP, 0, 0, _gprC, _value
                );
            }
            else {
                Assembler::current_section->getLiteralTable()->addLiteralReference(
                    _value, Assembler::current_section->size()
                );

                instruction = CREATE_INSTRUCTION(
                    (uint8_t) OP_CODE::ST, (uint8_t) MOD_ST::MEM_MEM_GPRA_GPRB_DISP, (uint32_t) GPR::PC, 0, _gprC, 0
                );
            }
            break;
        }
        case ADDR::REG_DIR: {
            instruction = CREATE_INSTRUCTION((uint8_t) OP_CODE::LD, (uint8_t) MOD_LD::GPR_DISP, _gprA, _gprB, 0, 0);
            break;
        }
        case ADDR::REG_IND: {
            instruction =
                CREATE_INSTRUCTION((uint8_t) OP_CODE::ST, (uint8_t) MOD_ST::MEM_GPRA_GPRB_DISP, _gprA, 0, _gprC, 0);
            break;
        }
        case ADDR::REG_IND_OFF: {
            if (_value > 0xFFF) {
                std::cout << std::dec << "Error at line " << line << ": ";
                std::cout << "offset is too large" << std::endl;
                exit(-1);
            }
            instruction = CREATE_INSTRUCTION(
                (uint8_t) OP_CODE::ST, (uint8_t) MOD_ST::MEM_GPRA_GPRB_DISP, _gprA, 0, _gprC, _value
            );
            break;
        }
        default:
            break;
    }

    Assembler::current_section->append(instruction);
}

void Instructions::store(ADDR _addr, uint8_t _gprA, uint8_t _gprB, uint8_t _gprC, std::string _symbol) {
    instruction_format instruction;

    Elf32_Sym* symbol_entry = Assembler::elf32_file->getSymbolTable().get(_symbol);

    if (symbol_entry == nullptr)
        symbol_entry = Assembler::elf32_file->getSymbolTable().add(_symbol, 0, false, Assembler::current_section->index());

    switch (_addr) {
        case ADDR::IMMEDIATE: {
            std::cout << std::dec << "Error at line " << line << ": ";
            std::cout << "immediate addressing usage in store instruction" << std::endl;
            exit(-1);
            break;
        }
        case ADDR::MEM_DIR: {
            Assembler::forward_reference_table.add(symbol_entry, Assembler::current_section->size());
            instruction = CREATE_INSTRUCTION(
                (uint8_t) OP_CODE::ST, (uint8_t) MOD_ST::MEM_MEM_GPRA_GPRB_DISP, (uint32_t) GPR::PC, 0, _gprC, 0
            );
            break;
        }
        default:
            break;
    }

    Assembler::current_section->append(instruction);
}

void Instructions::csr_store(uint8_t _csr, uint8_t _gpr) {
    Assembler::current_section->append(
        CREATE_INSTRUCTION((uint8_t) OP_CODE::LD, (uint8_t) MOD_LD::CSR, _gpr, _csr, 0, 0)
    );
}

void Instructions::exchange(uint8_t _gprS, uint8_t _gprD) {
    Assembler::current_section->append(CREATE_INSTRUCTION((uint8_t) OP_CODE::XCHG, 0, 0, _gprD, _gprS, 0));
}
