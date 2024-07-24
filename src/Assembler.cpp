#include "../inc/Assembler.hpp"

#include <fstream>
#include <iostream>

#include "../inc/Instructions.hpp"
#include "../inc/Section.hpp"
#include "../inc/SectionHeaderStringTable.hpp"

// Include the Flex and Bison headers to use their functions:
extern int yylex();
extern int yyparse();
extern FILE* yyin;

int Assembler::location_counter = 0;
Elf32_Ehdr* Assembler::elf_header = nullptr;
SectionHeaderTable* Assembler::section_header_table = nullptr;
SectionHeaderStringTable* Assembler::section_header_string_table = nullptr;

/** Function initAssembler should only be called once, at the beginning of the startAssembler,
 *  that's why it is private. It initializes the static variables of the Assembler class.
 */
void Assembler::initAssembler() {
    Assembler::elf_header = new Elf32_Ehdr();
    Assembler::section_header_table = new SectionHeaderTable();
    Assembler::section_header_string_table = &SectionHeaderStringTable::getInstance();
}

int Assembler::startAssembler() {
    Assembler::initAssembler();
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

    f_output.close();

    return 0;
}
