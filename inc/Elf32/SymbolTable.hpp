#pragma once

#include <deque>
#include <iostream>
#include <vector>

#include "Elf32.hpp"
#include "Section.hpp"

class Elf32File;

class SymbolTable : public Section {
public:

    SymbolTable(Elf32File* _elf32_file);

    void write(std::ofstream* _file);

    Elf32_Sym& add(const std::string& _name, Elf32_Sym _symbol_entry);
    Elf32_Sym& add(
        const std::string& _name, Elf32_Addr _value, bool _defined, Elf32_Half _section_index, unsigned char _info = 0
    );

    Elf32_Sym* get(const std::string& _name);
    Elf32_Sym* get(uint32_t _entry_index);

    std::deque<Elf32_Sym>& symbolTable();
    void replaceTable(const std::vector<Elf32_Sym>& _symbol_table);
    void changeValues(Elf32_Sym& _old_symbol, Elf32_Sym _new_symbol);

    void sort();

    uint32_t getIndex(const std::string& _name);
    uint32_t getIndex(Elf32_Sym& _symbol_entry);

    void defineSymbol(Elf32_Sym* _symbol_entry, Elf32_Addr _value, Elf32_Half _section_index);

    void print(std::ostream& _ostream) const;

    ~SymbolTable() = default;

private:

    std::deque<Elf32_Sym> symbol_table;
};
