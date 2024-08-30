#include "../inc/Assembler.hpp"

#include <cstddef>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "../inc/ElfHeader.hpp"
#include "../inc/Instructions.hpp"
#include "../inc/Section.hpp"
#include "../inc/ForwardReferenceTable.hpp"

// Include the Flex and Bison headers to use their functions:
extern int yylex();
extern int yyparse();
extern FILE* yyin;

CustomSection* Assembler::current_section;

std::ofstream Assembler::f_output;

ElfHeader* Assembler::elf_header;
SectionHeaderTable* Assembler::section_header_table;
StringTable* Assembler::string_table;
SymbolTable* Assembler::symbol_table;
ForwardReferenceTable* Assembler::forward_reference_table;

/** Function initAssembler should only be called once, at the beginning of the startAssembler,
 *  that's why it is private. It initializes the static variables of the Assembler class.
 */
int Assembler::startAssembler() {
    elf_header = new ElfHeader();
    section_header_table = new SectionHeaderTable();
    string_table = new StringTable();
    symbol_table = new SymbolTable();
    forward_reference_table = new ForwardReferenceTable();

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
    if (yyparse())
        exit(0);

    // Close the file handle:
    fclose(f_input);

    startBackpatching();

    return 0;
}

void Assembler::startBackpatching() {
    forward_reference_table->resolveSymbolForwardReferences();

    for (auto iterator : CustomSection::getSectionsMap()) {
        current_section = iterator.second;
        current_section->getLiteralTable().resolveReferences();
    }
}

int Assembler::writeToFile() {
    f_output.open("output.o", std::ios::out | std::ios::binary);

    if (!f_output.is_open()) {
        std::cout << "I can't open output.o!" << std::endl;
        return -1;
    }

    // Write sections right after the ELF header:
    f_output.seekp(sizeof(Elf32_Ehdr), std::ios::beg);

    for (auto iterator : CustomSection::getSectionsMap()) {
        iterator.second->write(&f_output);
    }

    // Write the string table:
    string_table->write(&f_output);

    // Write the symbol table:
    symbol_table->write(&f_output);

    // Set section header table offset and number of entries in the ELF header:
    std::streampos section_header_table_offset = f_output.tellp();
    elf_header->setField(ElfHeaderField::SH_OFFSET, section_header_table_offset);
    elf_header->setField(ElfHeaderField::SH_NUM, section_header_table->getSize());

    // Write the section header table:
    section_header_table->write(&f_output);

    // Write the ELF header at the beginning of the file:
    f_output.seekp(0, std::ios::beg);
    elf_header->write(&f_output);

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
            if (i % 16 == 0)
                std::cout << std::hex << std::setw(8) << std::setfill('0') << i << ": ";
            else if (i % 8 == 0)
                std::cout << std::dec << " ";

            std::cout << std::hex << std::setw(2) << std::setfill('0') << (unsigned int) (unsigned char) buffer[i]
                      << " ";

            if ((i + 1) % 16 == 0) {
                std::cout << std::dec << " |";
                for (size_t j = i - 15; j < i + 1; j++) {
                    if (buffer[j] < 32 || buffer[j] > 126) {
                        std::cout << ".";
                    }
                    else {
                        std::cout << buffer[j];
                    }
                    if ((j + 1) % 16 == 0) {
                        std::cout << std::dec << "|\n";
                    }
                }
            }
        }

        if (s % 16 != 0) {
            for (size_t i = 0; i < 16 - s % 16; i++) {
                std::cout << "   ";
                // adding another blank space after 8th byte
                if ((s + i) % 8 == 0) {
                    std::cout << " ";
                }
            }
            std::cout << std::dec << " |";
            for (size_t i = s - s % 16; i < s; i++) {
                if (buffer[i] < 32 || buffer[i] > 126) {
                    std::cout << ".";
                }
                else {
                    std::cout << buffer[i];
                }
                if (i == s - 1) {
                    std::cout << std::dec << "|\n";
                }
            }
        }
    }

    for (auto iterator : CustomSection::getSectionsMap()) {
        RelocationTable* relocation_table = &iterator.second->getRelocationTable();
        if (!relocation_table->isEmpty()) {
            relocation_table->print();
        }
    }

    symbol_table->print();
    section_header_table->print();

    std::cout << std::dec << std::endl;

    f_input.close();
}
