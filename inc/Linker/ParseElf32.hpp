#pragma once

#include <fstream>
#include <iostream>
#include <vector>

#include "../Elf32.hpp"
#include "../Elf32Header.hpp"
#include "../SectionHeaderTable.hpp"
#include "../SymbolTable.hpp"
#include "../StringTable.hpp"

class ParseElf32 {
public:

    // Constructor that opens the file and parses it
    ParseElf32(std::string _file);

    Elf32Header& getElf32Header();

    SectionHeaderTable& getSectionHeaderTable() ;

    SymbolTable& getSymbolTable();

    StringTable& getStringTable();

private:

    Elf32Header* elf32_header;
    SectionHeaderTable* section_header_table;
    SymbolTable* symbol_table;
    StringTable* string_table;

};