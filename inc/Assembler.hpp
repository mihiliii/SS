#pragma once

#include <iostream>
#include <vector>

#include "Section.hpp"
#include "InputSection.hpp"
#include "SectionHeaderTable.hpp"
#include "StringTable.hpp"
#include "SymbolTable.hpp"

class Assembler {
public:

    friend class Instructions;

    static void increaseLocationCounter(size_t _increment) { location_counter += _increment; }

    static size_t getLocationCounter() { return location_counter; }

    static void resetLocationCounter() { location_counter = 0; }

    static SectionHeaderTable* getSectionHeaderTable() { return section_header_table; }
    
    static int startAssembler();

    static int writeToFile();

    static void readElfFile();

    Assembler() = delete;

    ~Assembler() = delete;

    static InputSection* current_section;

    static std::ofstream f_output;

private:

    static void initAssembler() {};

    static size_t location_counter;
    static Elf32_Ehdr elf_header;
    static SectionHeaderTable* section_header_table;

    static StringTable* section_header_string_table;
    static SymbolTable* symbol_table;
};
