#pragma once

#include <iostream>
#include <vector>

#include "Section.hpp"
#include "SectionHeaderTable.hpp"
#include "SectionHeaderStringTable.hpp"
#include "SymbolTable.hpp"

class Assembler {
public:

    friend class Instructions;

    static void increaseLocationCounter() { location_counter++; }

    static void resetLocationCounter() { location_counter = 0; }

    static SectionHeaderTable* getSectionHeaderTable() { return section_header_table; }
    
    static int startAssembler();

    static int writeToFile();

    static void readElfFile();

    Assembler() = delete;

    ~Assembler() = delete;

    static Section* current_section;

    static std::ofstream f_output;

private:

    static void initAssembler() {};

    static int location_counter;
    static Elf32_Ehdr elf_header;
    static SectionHeaderTable* section_header_table;

    static SectionHeaderStringTable* section_header_string_table;
    static SymbolTable* symbol_table;
};
