#pragma once

#include <deque>
#include <iostream>
#include <vector>

#include "Elf32.hpp"
#include "Section.hpp"

class Elf32File;
class StringTable;

class SymbolTable : public Section {
public:

    friend class Elf32File;

    ~SymbolTable() = default;

    Elf32_Sym* get_symbol(const std::string& symbol_name);

    Elf32_Sym* get_symbol(Elf32_Word symbol_index);

    Elf32_Word get_symbol_index(const std::string& symbol_name);

    Elf32_Word get_symbol_index(const Elf32_Sym& symbol_entry);

    void set_symbol(Elf32_Sym& symbol_entry, Elf32_Sym new_symbol);

    void set_symbol(Elf32_Word symbol_index, Elf32_Sym new_symbol);

    Elf32_Sym& add_symbol(const std::string& symbol_name, Elf32_Sym symbol);

    Elf32_Sym& add_symbol(const std::string& symbol_name, Elf32_Addr value, bool defined,
                          Elf32_Half section_index, Elf32_Byte section_info = 0);

    void sort_data();

    void define_symbol(Elf32_Sym& symbol_entry, Elf32_Addr value, Elf32_Half section_index);

    void print(std::ostream& ostream) const;

    void write(std::ofstream& file) override;

private:

    SymbolTable(Elf32File& elf32_file);

    SymbolTable(const Elf32File& elf32_file) = delete;

    SymbolTable(Elf32File&& elf32_file) = delete;

    SymbolTable& operator=(const Elf32File& elf32_file) = delete;

    SymbolTable& operator=(Elf32File&& elf32_file) = delete;

    void replace_data(const std::vector<Elf32_Sym>& symbol_table);

    StringTable& _str_table;
    std::deque<Elf32_Sym> _sym_table;
};
