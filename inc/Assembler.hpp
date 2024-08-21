#pragma once

#include <iostream>
#include <vector>

#include "CustomSection.hpp"
#include "ElfHeader.hpp"
#include "Section.hpp"
#include "SectionHeaderTable.hpp"
#include "StringTable.hpp"
#include "SymbolTable.hpp"

class Assembler {
public:

    friend class Instructions;

    friend class Directives;

    static int startAssembler();

    static void startBackpatching();

    static int writeToFile();

    static void readElfFile();

    Assembler() = delete;

    ~Assembler() = delete;

    static CustomSection* current_section;

    static std::ofstream f_output;

    static ElfHeader* elf_header;
    static SectionHeaderTable* section_header_table;
    static StringTable* string_table;
    static SymbolTable* symbol_table;
};
