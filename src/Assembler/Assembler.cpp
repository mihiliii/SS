#include "../../inc/Assembler/Assembler.hpp"

#include <iostream>

#include "../../inc/Assembler/ForwardReferenceTable.hpp"
#include "../../inc/Assembler/LiteralTable.hpp"
#include "Elf32File.hpp"

// Include the Flex and Bison headers to use their functions:
extern int yylex();
extern int yyparse();
extern FILE* yyin;

// Define the global variables:
CustomSection* current_section = nullptr;
Elf32File elf32_file = Elf32File();
ForwardReferenceTable forward_reference_table = ForwardReferenceTable();
std::map<CustomSection*, LiteralTable> literal_table_map = std::map<CustomSection*, LiteralTable>();

int Assembler::startAssembler(const char* input_file_name, const char* output_file_name)
{
    // Open a file handle to a particular file:
    FILE* f_input = fopen(input_file_name, "r");

    // Make sure it is valid:
    if (!f_input) {
        std::cerr << "Error: can't open file " << std::string(input_file_name) << std::endl;
        return -1;
    }

    // Set Flex to read from it instead of defaulting to STDIN:
    yyin = f_input;

    // Parse through the input:
    if (yyparse()) {
        return -1;
    }

    // Close the file handle:
    fclose(f_input);

    // Backpatching phase:
    forward_reference_table.backpatch();
    for (auto iterator : literal_table_map) {
        iterator.second.resolve_references();
        iterator.second.addLiteralPoolToSection();
    }

    // Write the ELF file:
    elf32_file.write(std::string(output_file_name), ET_REL);
    elf32_file.readElf(std::string(output_file_name));

    return 0;
}
