#include "../../inc/Assembler/ForwardReferenceTable.hpp"

#include <cstdint>

#include "../../inc/Assembler/Assembler.hpp"
#include "../../inc/Assembler/LiteralTable.hpp"
#include "../../inc/CustomSection.hpp"
#include "../../inc/misc/Exceptions.hpp"
#include "Elf32.hpp"
#include "misc/Hardware.hpp"

using namespace Assembler;

ForwardReferenceTable::ForwardReferenceTable() : _forward_references()
{
}

// Adds a symbol reference of the symbol that will be resolved in backpatching phase.
void ForwardReferenceTable::add_reference(Elf32_Sym* _symbol_entry, Elf32_Addr _address)
{
    const std::string& symbol_name =
        elf32_file.get_string_table().get_string(_symbol_entry->st_name);

    if (_forward_references.find(symbol_name) == _forward_references.end()) {
        _forward_references[symbol_name] = std::list<SymbolReference>();
    }

    Elf32_Half current_section_index = Assembler::current_section->get_index();
    _forward_references[symbol_name].push_back({_address, current_section_index});
}

void ForwardReferenceTable::backpatch()
{
    for (auto& entry : _forward_references) {
        const std::string& symbol_name = entry.first;
        const std::list<SymbolReference>& list = entry.second;
        Elf32_Sym* symbol_entry = elf32_file.get_symbol_table().get_symbol(symbol_name);

        // Check if symbol is defined and local.
        if (symbol_entry->st_defined == false &&
            ELF32_ST_BIND(symbol_entry->st_info) == STB_LOCAL) {
            THROW_EXCEPTION("Symbol " + symbol_name + " is not defined.");
        }

        for (const SymbolReference& reference : list) {
            resolve_symbol(symbol_entry, reference);
        }
    }
}

void ForwardReferenceTable::resolve_symbol(Elf32_Sym* symbol_entry,
                                           const SymbolReference& reference)
{
    const Elf32_Off sh_name =
        elf32_file.get_section_header_table().at(reference.section_index).sh_name;
    CustomSection& section =
        elf32_file.get_custom_section_map().at(elf32_file.get_string_table().get_string(sh_name));

    instruction_t instruction = *(instruction_t*) section.get_data(reference.address);
    Elf32_Off offset = symbol_entry->st_value - reference.address - sizeof(instruction_t);

    const uint8_t op_mask = 0xF0;
    uint32_t instruction_op = INSTRUCTION_GET_OP_MODE(instruction) & op_mask;
    uint32_t instruction_mode = INSTRUCTION_GET_OP_MODE(instruction) & ~op_mask;
    const uint8_t jmp_op = (uint8_t) OpMode::JMP & op_mask;

    // For branch instructions jump location can be changed to an symbol directly depending if that
    // symbol definition is in the same section as the branch instruction. Other instructions have
    // to access symbol value using PC relative addressing mode, where the symbol value is located
    // in the literal table.
    if (instruction_op == jmp_op && offset < DispMask) {
        instruction_mode = (instruction_mode >= 0x8) ? (instruction_mode - 0x8) : instruction_mode;
        instruction = (instruction & 0xF00FF000) | (instruction_mode << OpModeShift) |
                      ((uint8_t) GPR::PC << RegAShift) | (offset & DispMask);

        section.overwrite_data(&instruction, sizeof(instruction_t), reference.address);
    } else {
        literal_table_map.at(&section).add_symbol_reference(symbol_entry, reference.address);
    }
}