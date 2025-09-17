#include "Assembler/ConstantTable.hpp"

#include "Assembler/InstructionFormat.hpp"

#include "Elf32/Elf32.hpp"
#include "Elf32/Elf32File.hpp"

#include <cstdint>

ConstantTable::ConstantTable(Elf32File& elf32_file, CustomSection& linked_section)
    : _elf32_file(elf32_file),
      _linked_section(linked_section) {};

// Method needs to be called when literal cant stay in instruction displacement field because of its
// size. Method adds literal to literal pool and adds a reference to that literal in the literal
// table. All references will be resolved in backpatching phase.
void ConstantTable::add_literal_reference(uint32_t literal, Elf32_Addr address)
{
    if (_literal_table.find(literal) == _literal_table.end()) {
        _literal_table[literal] =
            std::make_pair(_constant_pool.size() * sizeof(uint32_t), std::list<Elf32_Addr>());
        _constant_pool.emplace_back(literal);
    }

    std::list<Elf32_Addr>& list_reference_address = _literal_table[literal].second;
    list_reference_address.push_back(address);
}

void ConstantTable::add_symbol_reference(Elf32_Sym& symbol_entry, Elf32_Addr address)
{
    Elf32_Sym* symbol_entry_ptr = &symbol_entry;

    if (_symbol_value_table.find(symbol_entry_ptr) == _symbol_value_table.end()) {
        _symbol_value_table[symbol_entry_ptr] =
            std::make_pair(_constant_pool.size() * sizeof(uint32_t), std::list<Elf32_Addr>());

        Elf32_Word symbol_entry_index = _elf32_file.symbol_table.get_symbol_index(symbol_entry);

        _linked_section.get_rela_table().add_entry(
            _linked_section.get_size() + _constant_pool.size() * sizeof(uint32_t),
            ELF32_R_INFO(ELF32_R_TYPE_ABS32, symbol_entry_index), 0);
        _constant_pool.emplace_back(0);
    }

    std::list<Elf32_Addr>& list_reference_address = _symbol_value_table[symbol_entry_ptr].second;
    list_reference_address.push_back(address);
}

void ConstantTable::add_literal_pool_to_section()
{
    _linked_section.append_data(_constant_pool.data(), _constant_pool.size() * sizeof(uint32_t));
}

const std::vector<uint32_t>& ConstantTable::get_constant_pool() const
{
    return _constant_pool;
}

uint32_t ConstantTable::get_constant(size_t index) const
{
    return _constant_pool[index];
};

size_t ConstantTable::get_size() const
{
    return _constant_pool.size();
};

void ConstantTable::resolve_references()
{
    for (const auto& entry : _literal_table) {
        const std::list<Elf32_Addr>& list = entry.second.second;
        Elf32_Off literal_pool_offset = entry.second.first;

        for (const Elf32_Off& section_addr : list) {
            if (section_addr + 4 > _linked_section.get_size()) {
                throw std::runtime_error(
                    "CRITICAL: resolve_references: section address out of bounds");
            }

            instruction_format content = _linked_section.get_instruction(section_addr);

            uint32_t disp = literal_pool_offset + (_linked_section.get_size() - section_addr) - 4;

            if (disp > MAX_DISP) {
                throw std::runtime_error("literal pool overflow");
            }

            content = (content & ~IF_MASK_DISP) | (disp & IF_MASK_DISP);

            _linked_section.overwrite_data(&content, sizeof(instruction_format), section_addr);
        }
    }
    for (const auto& entry : _symbol_value_table) {
        const std::list<Elf32_Addr>& list = entry.second.second;
        Elf32_Off symbol_pool_offset = entry.second.first;

        for (const Elf32_Off& section_addr : list) {
            if (section_addr + 4 > _linked_section.get_size()) {
                throw std::runtime_error(
                    "CRITICAL: resolve_references: section address out of bounds");
            }

            instruction_format content = _linked_section.get_instruction(section_addr);

            uint32_t disp = symbol_pool_offset + (_linked_section.get_size() - section_addr) - 4;

            if (disp > MAX_DISP) {
                throw std::runtime_error("literal pool overflow");
            }

            content = (content & ~IF_MASK_DISP) | (disp & IF_MASK_DISP);

            _linked_section.overwrite_data(&content, sizeof(instruction_format), section_addr);
        }
    }
}
