#pragma once

#include <iostream>
#include <vector>

#include "../CustomSection.hpp"

struct Operand {
    std::string type;
    void* value;
};

class Assembler {
public:

    friend class Instructions;

    friend class Directives;

    static int startAssembler(const char* _input_file_name);

    static int writeToFile(const char* _output_file_name);

    static void readElfFile(const char* _input_file_name);

    static void startBackpatching();

    Assembler() = delete;

    ~Assembler() = delete;

    static CustomSection* current_section;

};
