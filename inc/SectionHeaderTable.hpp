#pragma once

#include <fstream>
#include <map>
#include <vector>

#include "Elf32.hpp"

class StringTable;
class SymbolTable;

class SectionHeaderTable {
public:

    SectionHeaderTable();

    SectionHeaderTable(std::vector<Elf32_Shdr> _section_header_table);

    void write(std::ofstream* _file);

    void print(std::ofstream& _file);

    uint32_t add();

    uint32_t add(Elf32_Shdr** _section_header);

    std::map<uint32_t, Elf32_Shdr*> getSectionHeaderTable() { return section_header_table; }

    Elf32_Shdr* getSectionHeader(uint32_t _index) { return section_header_table[_index]; }

    Elf32_Shdr* getSectionHeader(std::string _section_name);

    size_t getSize() const { return section_header_table.size() * sizeof(Elf32_Shdr); }

    StringTable* getStringTable() { return str_table; }

    SymbolTable* getSymbolTable() { return sym_table; }

    void setStringTable(StringTable* _str_table) { str_table = _str_table; }

    void setSymbolTable(SymbolTable* _sym_table) { sym_table = _sym_table; }

    ~SectionHeaderTable();

private:

    std::map<uint32_t, Elf32_Shdr*> section_header_table;
    uint32_t section_header_table_index;

    StringTable* str_table;
    SymbolTable* sym_table;

};
