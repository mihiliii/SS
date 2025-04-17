
#include <cstdint>
#include <string>

#include "../../inc/Assembler/Assembler.hpp"
#include "../../inc/Assembler/ForwardReferenceTable.hpp"
#include "../../inc/Assembler/LiteralTable.hpp"
#include "../../inc/Elf32File.hpp"
#include "../../inc/misc/Exceptions.hpp"
#include "../../inc/misc/Hardware.hpp"

extern uint32_t line;


bool equals(const Elf32_Sym& a, const Elf32_Sym& b)
{
    return a.st_name == b.st_name && a.st_value == b.st_value && a.st_size == b.st_size &&
           a.st_info == b.st_info && a.st_shndx == b.st_shndx;
}

void Assembler::Instruction::halt()
{
    current_section->add_data(INSTRUCTION_CREATE(OpMode::HALT, 0, 0, 0, 0));
}

void Assembler::Instruction::interrupt()
{
    current_section->add_data(INSTRUCTION_CREATE(OpMode::INT, 0, 0, 0, 0));
}

void Assembler::Instruction::iret()
{
    current_section->add_data(
        INSTRUCTION_CREATE(OpMode::LD_CSR_REGIND_DISP, CSR::STATUS, GPR::SP, 0, 4));

    current_section->add_data(INSTRUCTION_CREATE(OpMode::LD_GPR_POP, GPR::PC, GPR::SP, 0, 8));
}

void Assembler::Instruction::call(uint32_t _value)
{
    instruction_t instruction;

    if (_value < 0xFFF) {
        instruction = INSTRUCTION_CREATE(OpMode::CALL, 0, 0, 0, _value);
    } else {
        literal_table_map.at(current_section)
            .add_literal_reference(_value, current_section->get_size());

        instruction = INSTRUCTION_CREATE(OpMode::CALL_IND, GPR::PC, 0, 0, 0);
    }

    current_section->add_data(instruction);
}

void Assembler::Instruction::call(const std::string& _symbol)
{
    Elf32_Sym* symbol_entry = elf32_file.get_symbol_table().get_symbol(_symbol);

    if (symbol_entry == nullptr) {
        symbol_entry = &elf32_file.get_symbol_table().add_symbol(_symbol, 0, false,
                                                                 current_section->get_index());
    }

    forward_reference_table.add_reference(symbol_entry, current_section->get_size());
    current_section->add_data(INSTRUCTION_CREATE(OpMode::CALL_IND, GPR::PC, 0, 0, 0));
}

void Assembler::Instruction::alu(OpMode op_mode, GPR source, GPR destination)
{
    current_section->add_data(INSTRUCTION_CREATE(op_mode, destination, destination, source, 0));
}

void Assembler::Instruction::jump(OpMode op_mode, GPR gprA, GPR gprB, GPR gprC, uint32_t disp)
{
    instruction_t instruction;

    if (disp > 0xFFF) {
        op_mode = (OpMode) ((uint8_t) op_mode + 0x8);
        literal_table_map.at(current_section)
            .add_literal_reference(disp, current_section->get_size());
        instruction = INSTRUCTION_CREATE(op_mode, GPR::PC, gprB, gprC, 0);
    } else {
        instruction = INSTRUCTION_CREATE(op_mode, GPR::R0, gprB, gprC, disp);
    }

    current_section->add_data(instruction);
}

void Assembler::Instruction::jump(OpMode op_mode, GPR gprA, GPR gprB, GPR gprC,
                                  const std::string& symbol)
{
    const int ind_jump_mode = 0x8;

    if ((uint8_t) op_mode < ind_jump_mode) {
        op_mode = (OpMode) ((uint8_t) op_mode + 0x8);
    }

    Elf32_Sym* symbol_entry = elf32_file.get_symbol_table().get_symbol(symbol);

    if (symbol_entry == nullptr) {
        symbol_entry = &elf32_file.get_symbol_table().add_symbol(symbol, 0, false,
                                                                 current_section->get_index());
    }

    forward_reference_table.add_reference(symbol_entry, current_section->get_size());
    instruction_t instruction = INSTRUCTION_CREATE(op_mode, GPR::PC, gprB, gprC, 0);

    current_section->add_data(instruction);
}

void Assembler::Instruction::push(GPR gpr)
{
    current_section->add_data(INSTRUCTION_CREATE(OpMode::ST_PUSH, GPR::SP, 0, gpr, -4));
}

void Assembler::Instruction::pop(GPR gpr)
{
    current_section->add_data(INSTRUCTION_CREATE(OpMode::LD_GPR_POP, gpr, GPR::SP, 0, 4));
}

void Assembler::Instruction::load(AddrMode addr_mode, GPR gprA, GPR gprB, uint32_t disp)
{
    instruction_t instruction;

    switch (addr_mode) {
        case AddrMode::IMMEDIATE: {
            if (disp < 0xFFF) {
                instruction = INSTRUCTION_CREATE(OpMode::LD_GPR_GPR_DISP, gprA, 0, 0, disp);
            } else {
                literal_table_map.at(current_section)
                    .add_literal_reference(disp, current_section->get_size());

                instruction = INSTRUCTION_CREATE(OpMode::LD_GPR_REGIND_DISP, gprA, GPR::PC, 0, 0);
            }
            break;
        }
        case AddrMode::MEM_DIR: {
            if (disp < 0xFFF) {
                instruction = INSTRUCTION_CREATE(OpMode::LD_GPR_REGIND_DISP, gprA, 0, 0, disp);
            } else {
                literal_table_map.at(current_section)
                    .add_literal_reference(disp, current_section->get_size());

                instruction = INSTRUCTION_CREATE(OpMode::LD_GPR_REGIND_DISP, gprA, GPR::PC, 0, 0);

                current_section->add_data(instruction);

                instruction = INSTRUCTION_CREATE(OpMode::LD_GPR_REGIND_DISP, gprA, gprA, 0, 0);
            }
            break;
        }
        case AddrMode::REG_DIR: {
            instruction = INSTRUCTION_CREATE(OpMode::LD_GPR_GPR_DISP, gprA, gprB, 0, 0);
            break;
        }
        case AddrMode::REG_IND: {
            instruction = INSTRUCTION_CREATE(OpMode::LD_CSR_REGIND_DISP, gprA, gprB, 0, 0);
            break;
        }
        case AddrMode::REG_IND_OFF: {
            if (disp > 0xFFF) {
                THROW_EXCEPTION("Offset is too large");
            }
            instruction = INSTRUCTION_CREATE(OpMode::LD_GPR_REGIND_DISP, gprA, gprB, 0, disp);
        }
        default:
            THROW_EXCEPTION("Invalid addressing mode");
            break;
    }

    current_section->add_data(instruction);
}

void Assembler::Instruction::load(AddrMode addr_mode, GPR gprA, GPR gprB, const std::string& symbol)
{
    instruction_t instruction;

    Elf32_Sym* symbol_entry = elf32_file.get_symbol_table().get_symbol(symbol);

    if (symbol_entry == nullptr)
        symbol_entry = &elf32_file.get_symbol_table().add_symbol(symbol, 0, false,
                                                                 current_section->get_index());

    switch (addr_mode) {
        case AddrMode::IMMEDIATE: {
            forward_reference_table.add_reference(symbol_entry, current_section->get_size());
            instruction =
                INSTRUCTION_CREATE(OpMode::LD_GPR_REGIND_DISP, gprA, (uint32_t) GPR::PC, 0, 0);
            break;
        }
        case AddrMode::MEM_DIR: {
            forward_reference_table.add_reference(symbol_entry, current_section->get_size());
            instruction =
                INSTRUCTION_CREATE(OpMode::LD_GPR_REGIND_DISP, gprA, (uint32_t) GPR::PC, 0, 0);

            current_section->add_data(instruction);

            instruction = INSTRUCTION_CREATE(OpMode::LD_GPR_REGIND_DISP, gprA, gprA, 0, 0);
            break;
        }
        default:
            THROW_EXCEPTION("Invalid addressing mode");
            break;
    }

    current_section->add_data(instruction);
}

void Assembler::Instruction::csr_read(CSR csr, GPR gpr)
{
    current_section->add_data(INSTRUCTION_CREATE(OpMode::LD_GPR_CSR, gpr, csr, 0, 0));
}

void Assembler::Instruction::store(AddrMode addr_mode, GPR gprA, GPR gprB, GPR gprC, uint32_t disp)
{
    instruction_t instruction;

    switch (addr_mode) {
        case AddrMode::IMMEDIATE: {
            THROW_EXCEPTION("Immediate addressing usage in store instruction");
            break;
        }
        case AddrMode::MEM_DIR: {
            if (disp < 0xFFF) {
                instruction = INSTRUCTION_CREATE(OpMode::ST_REGIND_DISP, 0, 0, gprC, disp);
            } else {
                literal_table_map.at(current_section)
                    .add_literal_reference(disp, current_section->get_size());

                instruction = INSTRUCTION_CREATE(OpMode::ST_MEM_REGIND_DISP, GPR::PC, 0, gprC, 0);
            }
            break;
        }
        case AddrMode::REG_DIR: {
            instruction = INSTRUCTION_CREATE(OpMode::LD_GPR_GPR_DISP, gprA, gprB, 0, 0);
            break;
        }
        case AddrMode::REG_IND: {
            instruction = INSTRUCTION_CREATE(OpMode::ST_REGIND_DISP, gprA, 0, gprC, 0);
            break;
        }
        case AddrMode::REG_IND_OFF: {
            if (disp > 0xFFF) {
                THROW_EXCEPTION("Offset is too large");
            }
            instruction = INSTRUCTION_CREATE(OpMode::ST_REGIND_DISP, gprA, 0, gprC, disp);
            break;
        }
        default:
            THROW_EXCEPTION("Invalid addressing mode");
            break;
    }

    current_section->add_data(instruction);
}

void Assembler::Instruction::store(AddrMode addr_mode, GPR gprA, GPR gprB, GPR gprC,
                                   const std::string& symbol)
{
    instruction_t instruction;

    Elf32_Sym* symbol_entry = elf32_file.get_symbol_table().get_symbol(symbol);

    if (symbol_entry == nullptr) {
        symbol_entry = &elf32_file.get_symbol_table().add_symbol(symbol, 0, false,
                                                                 current_section->get_index());
    }

    switch (addr_mode) {
        case AddrMode::IMMEDIATE: {
            THROW_EXCEPTION("Immediate addressing usage in store instruction");
            break;
        }
        case AddrMode::MEM_DIR: {
            forward_reference_table.add_reference(symbol_entry, current_section->get_size());
            instruction = INSTRUCTION_CREATE(OpMode::ST_MEM_REGIND_DISP, GPR::PC, 0, gprC, 0);
            break;
        }
        default:
            THROW_EXCEPTION("Invalid addressing mode");
            break;
    }

    current_section->add_data(instruction);
}

void Assembler::Instruction::csr_write(GPR gpr, CSR csr)
{
    current_section->add_data(INSTRUCTION_CREATE(OpMode::LD_CSR_GPR, csr, gpr, 0, 0));
}

void Assembler::Instruction::exchange(GPR gprB, GPR gprC)
{
    current_section->add_data(INSTRUCTION_CREATE(OpMode::XCHG, 0, gprB, gprC, 0));
}
