#include "../inc/Assembler.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <cstddef>

#include "../inc/Instructions.hpp"
#include "../inc/Section.hpp"
#include "../inc/ElfHeader.hpp"

// Include the Flex and Bison headers to use their functions:
extern int yylex();
extern int yyparse();
extern FILE* yyin;

size_t Assembler::location_counter = 0;
InputSection* Assembler::current_section;

SectionHeaderTable* Assembler::section_header_table = &SectionHeaderTable::getInstance();
ElfHeader* Assembler::elf_header = &ElfHeader::getInstance();

std::ofstream Assembler::f_output;

/** Function initAssembler should only be called once, at the beginning of the startAssembler,
 *  that's why it is private. It initializes the static variables of the Assembler class.
 */
int Assembler::startAssembler() {
    SymbolTable::getInstance();
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
    f_output.open("output.o", std::ios::out | std::ios::binary);

    std::cout << sizeof(Elf32_Ehdr) << std::endl;

    if (!f_output.is_open()) {
        std::cout << "I can't open output.o!" << std::endl;
        return -1;
    }

    // Write the ELF header to the file:
    elf_header->write(&f_output);

    // Write the section header table to the file:
    std::streampos section_header_table_offset = f_output.tellp();
    section_header_table->writeFile(&f_output);

    // Write the section header table offset to the ELF header:
    f_output.seekp(offsetof(Elf32_Ehdr, e_shoff), std::ios::beg);
    f_output.write(reinterpret_cast<char*>(&section_header_table_offset), sizeof(std::streampos));

    // Write the string table to the file:
    f_output.seekp(0, std::ios::end);
    std::streampos string_table_offset = f_output.tellp();

    string_table->write(&f_output);

    f_output.close();

    return 0;
}

void Assembler::readElfFile() {
    std::ifstream f_input("output.o", std::ios::in | std::ios::binary);

    if (!f_input.is_open()) {
        std::cout << "I can't open output.o!" << std::endl;
        return;
    }

    size_t bufferSize = 1024;
    std::vector<char> buffer(bufferSize);

    std::cout << "Content of output.o:\n";

    while (!f_input.eof()) {
        f_input.read(buffer.data(), bufferSize);
        size_t s = f_input.gcount();
        for (size_t i = 0; i < s; i++) {
            if (i % 16 == 0) {
                std::cout << std::hex << std::setw(8) << std::setfill('0') << i << ": ";
            }
            std::cout << std::hex << std::setw(2) << std::setfill('0')
                      << (unsigned int) (unsigned char) buffer[i] << " ";
            if ((i + 1) % 16 == 0) {
                std::cout << std::dec << "\n";
            }
        }
    }

    std::cout << std::dec << "\n";

    f_input.clear();
    f_input.seekg(0, std::ios::beg);

    while (!f_input.eof()) {
        f_input.read(buffer.data(), bufferSize);
        size_t s = f_input.gcount();
        for (size_t i = 0; i < s; i++) {
            if (i % 16 == 0) {
                std::cout << std::hex << std::setw(8) << std::setfill('0') << i << ": ";
            }
            std::cout << buffer[i] << " ";
            if ((i + 1) % 16 == 0) {
                std::cout << std::dec << "\n";
            }
        }
    }

    f_input.close();
}
