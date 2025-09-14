#pragma once

#include <deque>
#include <iostream>
#include <vector>

#include "Section.hpp"

class SymbolTable : public Section {
public:

    SymbolTable(Elf32File& elf32_file);

    SymbolTable(const SymbolTable&) = delete;

    SymbolTable(SymbolTable&&) = default;

    SymbolTable& operator=(const SymbolTable&) = delete;

    SymbolTable& operator=(SymbolTable&&) = default;

    ~SymbolTable() = default;

    Elf32_Sym& add_symbol(const std::string& name, Elf32_Sym symbol_entry);

    Elf32_Sym& add_symbol(const std::string& name, Elf32_Addr value, bool defined,
                          Elf32_Half section_index, unsigned char info = 0);

    Elf32_Sym* find_symbol(const std::string& name);

    Elf32_Sym* find_symbol(Elf32_Word entry_index);

    Elf32_Sym& get_symbol(const std::string& name);

    Elf32_Sym& get_symbol(Elf32_Word entry_index);

    Elf32_Word get_symbol_index(const std::string& name);

    Elf32_Word get_symbol_index(Elf32_Sym& symbol_entry);

    std::deque<Elf32_Sym>& get_symbol_table();

    void set_symbol(Elf32_Sym& table_entry, const std::string& symbol_name, Elf32_Byte st_info,
                    Elf32_Half st_shndx, Elf32_Addr st_value, Elf32_Word st_size, bool st_defined);

    void set_symbol(Elf32_Sym& table_entry, const std::string& symbol_name, Elf32_Sym symbol);

    void define_symbol(Elf32_Sym& symbol_entry, Elf32_Addr value, Elf32_Half section_index);

    void set_symbol_table(const std::vector<Elf32_Sym>& symbol_table);

    void sort();

    void write(std::ostream& file);

    void print(std::ostream& ostream) const;

private:

    std::deque<Elf32_Sym> _symbol_table;
};
