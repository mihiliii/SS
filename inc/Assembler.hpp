#pragma once

#include <iostream>
#include <vector>

#include "ElfHeader.hpp"
#include "InputSection.hpp"
#include "Section.hpp"
#include "SectionHeaderTable.hpp"
#include "StringTable.hpp"
#include "SymbolTable.hpp"

class Assembler {
public:

    friend class Instructions;

    friend class Directives;

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

    static ElfHeader* elf_header;
    static SectionHeaderTable* section_header_table;

    static StringTable* string_table;
    static SymbolTable* symbol_table;
};
