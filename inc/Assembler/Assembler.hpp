#pragma once

#include <iostream>
#include <vector>
#include <map>

class CustomSection; 
class Elf32File;
class ForwardReferenceTable;
class LiteralTable;

struct Operand {
    std::string type;
    void* value;
};

class Assembler {
public:

    friend class Instructions;

    friend class Directives;

    static int startAssembler(const char* _input_file_name, const char* _output_file_name);

    static void startBackpatching();

    static void closeAssembler();

    Assembler() = delete; 

    ~Assembler() = delete;

    static CustomSection* current_section;

    static Elf32File* elf32_file;
    static ForwardReferenceTable forward_reference_table;
    static std::map<CustomSection*, LiteralTable> literal_table_map;

};
