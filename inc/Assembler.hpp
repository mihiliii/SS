#ifndef Assembler_hpp_
#define Assembler_hpp_

#include <iostream>

#include "Section.hpp"
#include "SectionHeaderTable.hpp"
#include "SectionHeaderStringTable.hpp"
#include "SymbolTable.hpp"

class Assembler {
public:

    friend class Instructions;

    template <typename T>
    friend class Section;

    static void increaseLocationCounter() { location_counter++; }

    static SectionHeaderTable* getSectionHeaderTable() { return section_header_table; }
    
    static int startAssembler();

    static int writeToFile();

    Assembler() = delete;

    ~Assembler() = delete;

private:

    static void initAssembler();

    static int location_counter;
    static Elf32_Ehdr* elf_header;
    static SectionHeaderTable* section_header_table;

    static SectionHeaderStringTable* section_header_string_table;
    static SymbolTable* symbol_table;
};

#endif
