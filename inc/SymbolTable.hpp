#pragma once

#include <iostream>
#include <list>
#include <map>
#include <vector>

#include "Elf32.hpp"
#include "Section.hpp"

class Elf32File;

class SymbolTable : public Section {
public:

    friend class ForwardReferenceTable;

    SymbolTable(Elf32File* _elf32_file);
    SymbolTable(Elf32File* _elf32_file, Elf32_Shdr _section_header, std::vector<Elf32_Sym> _symbol_table);

    void write(std::ofstream* _file);

    Elf32_Sym* add(std::string _name, Elf32_Sym _symbol_entry);
    Elf32_Sym* add(
        std::string _name, Elf32_Addr _value, bool _defined, Elf32_Half _section_index, unsigned char _info = 0
    );

    Elf32_Sym* get(std::string _name);
    Elf32_Sym* get(uint32_t _entry_index);

    std::vector<Elf32_Sym*>& getContent();

    uint32_t getIndex(std::string _name);
    uint32_t getIndex(Elf32_Sym* _symbol_entry);

    void defineSymbol(Elf32_Sym* _symbol_entry, Elf32_Addr _value);

    void print(std::ofstream& _file) const;

    ~SymbolTable();

private:

    std::vector<Elf32_Sym*> symbol_table;
};
