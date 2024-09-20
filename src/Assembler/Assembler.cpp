#include "../../inc/Assembler/Assembler.hpp"

#include <cstddef>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "../../inc/Assembler/ForwardReferenceTable.hpp"
#include "../../inc/LiteralTable.hpp"
#include "../../inc/Elf32File.hpp"

// Include the Flex and Bison headers to use their functions:
extern int yylex();
extern int yyparse();
extern FILE* yyin;

CustomSection* Assembler::current_section = nullptr;

Elf32File* Assembler::elf32_file = nullptr;
ForwardReferenceTable Assembler::forward_reference_table;

int Assembler::startAssembler(const char* _input_file_name, const char* _output_file_name) {
    // Create an ELF file:
    elf32_file = new Elf32File();

    // Open a file handle to a particular file:
    FILE* f_input = fopen(_input_file_name, "r");
    // Make sure it is valid:
    if (!f_input) {
        std::cerr << "Error: can't open file " << std::string(_input_file_name) << std::endl;
        return -1;
    }
    // Set Flex to read from it instead of defaulting to STDIN:
    yyin = f_input;

    // Parse through the input:
    if (yyparse())
        return -1;

    // Close the file handle:
    fclose(f_input);

    startBackpatching();

    // Write the ELF file:
    elf32_file->write(std::string(_output_file_name), ET_REL);
    elf32_file->readElf(std::string(_output_file_name));

    return 0;
}

void Assembler::startBackpatching() {
    forward_reference_table.backpatch();

    for (auto iterator : elf32_file->getCustomSections()) {
        current_section = iterator.second;
        current_section->getLiteralTable()->resolveReferences();
    }
}

void Assembler::closeAssembler() {
    delete elf32_file;
}
