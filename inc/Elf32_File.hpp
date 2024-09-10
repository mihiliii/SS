#pragma once

#include <map>
#include "Elf32.hpp"
#include "StringTable.hpp"
#include "SymbolTable.hpp"
#include "CustomSection.hpp"

typedef std::vector<Elf32_Shdr*> SectionHeaderTable;
typedef Elf32_Ehdr Elf32Header;

class Elf32_File {
public:

    Elf32_File();
    Elf32_File(std::string _file_name);
    Elf32_File(Elf32Header _elf32_header, SectionHeaderTable _sh_table, StringTable _str_table, SymbolTable _sym_table);
    Elf32_File(Elf32Header _elf32_header, SectionHeaderTable _sh_table, StringTable _str_table, SymbolTable _sym_table, std::map<std::string, CustomSection*> _custom_sections);

    void write(std::string _file_name);
    void write(std::ofstream* _file);

    Elf32Header& getElf32Header() { return elf32_header; }
    SectionHeaderTable& getSectionHeaderTable() { return sh_table; }
    StringTable& getStringTable() { return str_table; }
    SymbolTable& getSymbolTable() { return sym_table; }
    std::map<std::string, CustomSection*>& getCustomSections() { return custom_sections; }

    ~Elf32_File();

private:

    Elf32Header elf32_header;
    SectionHeaderTable sh_table;
    StringTable str_table;   
    SymbolTable sym_table;
    std::map<std::string, CustomSection*> custom_sections;

};