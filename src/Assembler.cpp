#include "../inc/Assembler.hpp"

#include <fstream>
#include <iostream>

#include "../inc/Instructions.hpp"
#include "../inc/SectionContent.hpp"

// Include the Flex and Bison headers to use their functions:
extern int   yylex();
extern int   yyparse();
extern FILE* yyin;

int Assembler::location_counter = 0;

SectionContent* Assembler::text_section = new SectionContent();

int Assembler::startAssembler() {
    // Open a file handle to a particular file:
    FILE* f_input = fopen("input.txt", "r");
    // Make sure it is valid:
    if (!f_input) {
        std::cout << "I can't open input.txt!" << std::endl;
        return -1;
    }
    // Set Flex to read from it instead of defaulting to STDIN:
    yyin = f_input;

    // Parse through the input:
    yyparse();

    // Close the file handle:
    fclose(f_input);

    return 0;
}

int Assembler::writeToFile() {
    std::ofstream f_output("output.o");

    if (!f_output.is_open()) {
        std::cout << "I can't open output.o!" << std::endl;
        return -1;
    }

    for (uint8_t i: *text_section->getContent()) {
        f_output << i;
    }

    f_output.close();

    return 0;
}

void Assembler::decodeInstruction(const std::string& string) {
    Instructions::instruction_map[string]();
}

void Assembler::decodeInstruction(char* string) { Instructions::instruction_map[string](); }
