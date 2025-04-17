#include "../../inc/Assembler/LiteralTable.hpp"

#include <cstdint>

#include "../../inc/CustomSection.hpp"
#include "../../inc/Elf32.hpp"
#include "../../inc/Elf32File.hpp"
#include "../../inc/RelocationTable.hpp"
#include "../../inc/SymbolTable.hpp"
#include "misc/Exceptions.hpp"

LiteralTable::LiteralTable(Elf32File& elf32_file, CustomSection& linked_section)
    : _elf32_file(elf32_file), _linked_section(linked_section) {};

// Method needs to be called when literal cant stay in instruction displacement field because of its
// size. Method adds literal to literal pool and adds a reference to that literal in the literal
// table. All references will be resolved in backpatching phase.
void LiteralTable::add_literal_reference(int literal, Elf32_Addr address)
{
    if (_literal_references.find(literal) == _literal_references.end()) {
        _literal_references[literal] = {(Elf32_Off) (get_pool_size()), std::list<Elf32_Addr>()};
        _data_pool.emplace_back(literal);
    }
    _literal_references[literal].addresses.push_back(address);
}

void LiteralTable::add_symbol_reference(Elf32_Sym* symbol_entry, Elf32_Addr address)
{
    if (_symbol_references.find(symbol_entry) == _symbol_references.end()) {
        _symbol_references[symbol_entry] = {(Elf32_Off) (get_pool_size()), std::list<Elf32_Addr>()};
        Elf32_Word index = _elf32_file.get_symbol_table().get_symbol_index(*symbol_entry);

        _linked_section.get_relocation_table().add_entry(
            _linked_section.get_size() + get_pool_size(), ELF32_R_INFO(ELF32_R_TYPE_ABS32, index),
            0);
        _data_pool.emplace_back(0);
    }
    _symbol_references[symbol_entry].addresses.push_back(address);
}

void LiteralTable::addLiteralPoolToSection()
{
    _linked_section.add_data(_data_pool.data(), get_pool_size());
}

uint32_t* LiteralTable::get_literal(size_t index)
{
    return &_data_pool[index];
}

size_t LiteralTable::get_pool_size() const
{
    return _data_pool.size() * sizeof(uint32_t);
}

void LiteralTable::resolve_references()
{
    for (const auto& literal_reference : _literal_references) {
        for (const Elf32_Off& section_offset : literal_reference.second.addresses) {
            Elf32_Off pool_offset = literal_reference.second.pool_offset;
            uint8_t* content = (uint8_t*) _linked_section.get_data(section_offset);

            Elf32_Off disp =
                pool_offset + (_linked_section.get_size() - section_offset) - sizeof(instruction_t);

            if (disp > 0xFFF) {
                THROW_EXCEPTION("Literal pool overflow");
            }

            instruction_t new_content =
                content[3] << 24 | content[2] << 16 | (content[1] & 0xF0) << 8 | (disp & 0xFFF);

            _linked_section.overwrite_data(&new_content, sizeof(instruction_t), section_offset);
        }
    }
    for (const auto& entry : _symbol_references) {
        for (const Elf32_Off& section_offset : entry.second.addresses) {
            Elf32_Off pool_offset = entry.second.pool_offset;
            uint8_t* content = (uint8_t*) _linked_section.get_data(section_offset);

            Elf32_Off disp =
                pool_offset + (_linked_section.get_size() - section_offset) - sizeof(instruction_t);

            if (disp > 0xFFF) {
                THROW_EXCEPTION("Literal pool overflow");
            }

            instruction_t new_content =
                content[3] << 24 | content[2] << 16 | (content[1] & 0xF0) << 8 | (disp & 0xFFF);

            _linked_section.overwrite_data(&new_content, sizeof(instruction_t), section_offset);
        }
    }
}
