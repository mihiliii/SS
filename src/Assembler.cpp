#include "../inc/Assembler.hpp"
#include "../inc/SectionContent.hpp"
#include <iostream>

// Include the Flex and Bison headers to use their functions:
extern int yylex();
extern int yyparse();
extern FILE *yyin;

int Assembler::location_counter = 0;

SectionContent* Assembler::text_section = new SectionContent();

int Assembler::startAssembler() {
    // Open a file handle to a particular file:
    FILE *myfile = fopen("input.txt", "r");
    // Make sure it is valid:
    if (!myfile) {
        std::cout << "I can't open input.txt!" << std::endl;
        return -1;
    }
    // Set Flex to read from it instead of defaulting to STDIN:
    yyin = myfile;
  
    // Parse through the input:
    yyparse();
  
    return 0;
}
