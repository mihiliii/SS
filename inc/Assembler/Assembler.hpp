#pragma once

#include <iostream>
#include <vector>

#include "../CustomSection.hpp"
#include "../Elf32Header.hpp"
#include "../SectionHeaderTable.hpp"
#include "../StringTable.hpp"
#include "../SymbolTable.hpp"
#include "ForwardReferenceTable.hpp"

struct Operand {
    std::string type;
    void* value;
};

class Assembler {
public:

    friend class Instructions;

    friend class Directives;

    static int startAssembler(const char* _input_file_name);

    static int writeToBinFile(const char* _output_file_name);

    static int writeToTxtFile(const char* _input_file_name);

    static void startBackpatching();

    static void closeAssembler();

    Assembler() = delete; 

    ~Assembler() = delete;

    static CustomSection* current_section;

    static Elf32Header* elf32_header;
    static SectionHeaderTable* section_header_table;
    static StringTable* string_table;
    static SymbolTable* symbol_table;
    static ForwardReferenceTable* forward_reference_table;

};
