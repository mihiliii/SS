#pragma once

#include <deque>
#include <iostream>
#include <vector>

#include "Elf32.hpp"
#include "Section.hpp"

class Elf32File;

class SymbolTable : public Section {
public:

    SymbolTable(Elf32File& elf32_file);

    Elf32_Sym& add_symbol(const std::string& name, Elf32_Sym symbol_entry);

    Elf32_Sym& add_symbol(const std::string& name, Elf32_Addr value, bool defined,
                          Elf32_Half section_index, unsigned char info = 0);

    Elf32_Sym* get_symbol(const std::string& name);

    Elf32_Sym* get_symbol(Elf32_Word entry_index);

    std::deque<Elf32_Sym>& get_symbol_table(); // NOTE: CHECK IF NEEDED

    void replace_table(const std::vector<Elf32_Sym>& symbol_table);

    void change_values(Elf32_Sym& old_symbol, Elf32_Sym new_symbol);

    void sort(); // NOTE: CHECK IF NEEDED

    Elf32_Word get_index(const std::string& name);

    Elf32_Word get_index(Elf32_Sym& symbol_entry);

    void define_symbol(Elf32_Sym* symbol_entry, Elf32_Addr value, Elf32_Half section_index);

    void print(std::ostream& ostream) const;

    void write(std::ofstream* file);

    ~SymbolTable() = default;

private:

    std::deque<Elf32_Sym> _symbol_table;
};
