#pragma once

#include <cstdint>
#include <list>
#include <unordered_map>
#include <vector>

#include "../Elf32/Elf32File.hpp"

typedef std::unordered_map<uint32_t, std::pair<Elf32_Off, std::list<Elf32_Addr>>> LiteralTable;
typedef std::unordered_map<Elf32_Sym*, std::pair<Elf32_Off, std::list<Elf32_Addr>>>
    SymbolValueTable;

class ConstantTable {
public:

    ConstantTable(Elf32File& elf32_file, CustomSection& linked_section);

    void add_literal_reference(uint32_t literal, Elf32_Addr address);

    void add_symbol_reference(Elf32_Sym& symbol_entry, Elf32_Addr address);

    void add_literal_pool_to_section();

    const std::vector<uint32_t>& get_constant_pool() const;

    uint32_t get_constant(size_t index) const;

    size_t get_size() const;

    void resolve_references();

    ~ConstantTable() = default;

private:

    Elf32File& _elf32_file;
    CustomSection& _linked_section;

    LiteralTable _literal_table;
    SymbolValueTable _symbol_value_table;
    std::vector<uint32_t> _constant_pool;
};
