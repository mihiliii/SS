#include "../inc/Assembler/Assembler.hpp"

#include <cstddef>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "../inc/Assembler/ForwardReferenceTable.hpp"
#include "../inc/Assembler/Instructions.hpp"
#include "../inc/Elf32Header.hpp"
#include "../inc/Section.hpp"
#include "../inc/SectionHeaderTable.hpp"
#include "../inc/StringTable.hpp"

// Include the Flex and Bison headers to use their functions:
extern int yylex();
extern int yyparse();
extern FILE* yyin;

CustomSection* Assembler::current_section;

int Assembler::startAssembler(const char* _input_file_name) {

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
        exit(0);

    // Close the file handle:
    fclose(f_input);

    startBackpatching();

    return 0;
}

void Assembler::startBackpatching() {
    ForwardReferenceTable::getInstance().backpatch();

    for (auto iterator : CustomSection::getSectionsMap()) {
        current_section = iterator.second;
        current_section->getLiteralTable().resolveReferences();
    }
}

int Assembler::writeToFile(const char* _output_file_name) {
    std::ofstream f_output(_output_file_name, std::ios::out | std::ios::binary);

    if (!f_output.is_open()) {
        std::cerr << "Error: can't open file " << std::string(_output_file_name) << std::endl;
        return -1;
    }

    // Write sections right after the ELF header:
    f_output.seekp(sizeof(Elf32_Ehdr), std::ios::beg);

    for (auto iterator : CustomSection::getSectionsMap()) {
        iterator.second->write(&f_output);
    }

    // Write the string table:
    StringTable::getInstance().write(&f_output);

    // Write the symbol table:
    SymbolTable::getInstance().write(&f_output);

    // Set section header table offset and number of entries in the ELF header:
    std::streampos section_header_table_offset = f_output.tellp();
    Elf32Header::getInstance().setField(Elf32_Ehdr_Field::e_type, ET_REL);
    Elf32Header::getInstance().setField(Elf32_Ehdr_Field::e_shoff, section_header_table_offset);
    Elf32Header::getInstance().setField(Elf32_Ehdr_Field::e_shnum, SectionHeaderTable::getInstance().getSize() / sizeof(Elf32_Shdr));

    // Write the section header table:
    SectionHeaderTable::getInstance().write(&f_output);

    // Write the ELF header at the beginning of the file:
    f_output.seekp(0, std::ios::beg);
    Elf32Header::getInstance().write(&f_output);

    f_output.close();

    return 0;
}

void Assembler::readElfFile(const char* _input_file_name) {
    std::ifstream f_input(_input_file_name, std::ios::in | std::ios::binary);
    std::ofstream f_output("readelf.txt", std::ios::out);

    if (!f_input.is_open()) {
        std::cout << "Error: can't open " << _input_file_name << std::endl;
        return;
    }
    else if (!f_output.is_open()) {
        std::cout << "Error: can't open readelf.txt" << std::endl;
        return;
    }

    size_t bufferSize = 1024;
    std::vector<char> buffer(bufferSize);

    int len = (90 - (std::string(_input_file_name).length() + 9)) / 2;

    f_output << std::setw(len) << std::setfill('*') << "   " << "ELFREAD: " << _input_file_name << std::setw(len)
             << std::setfill('*') << std::left << "   " << std::endl;

    Elf32Header::getInstance().print(f_output);
    SectionHeaderTable::getInstance().print(f_output);

    for (auto iterator : CustomSection::getSectionsMap()) {
        RelocationTable& relocation_table = iterator.second->getRelocationTable();
        if (!relocation_table.isEmpty()) {
            relocation_table.print(f_output);
        }
        iterator.second->getLiteralTable().print(f_output);
    }

    SymbolTable::getInstance().print(f_output);

    f_output << std::endl << "Content of file: " << _input_file_name << ":\n";
    while (!f_input.eof()) {
        f_input.read(buffer.data(), bufferSize);
        size_t s = f_input.gcount();
        for (size_t i = 0; i < s; i++) {
            if (i % 16 == 0)
                f_output << std::right << std::hex << std::setw(8) << std::setfill('0') << i << ": ";
            else if (i % 8 == 0)
                f_output << std::dec << " ";

            f_output << std::hex << std::setw(2) << std::setfill('0') << (unsigned int) (unsigned char) buffer[i]
                     << " ";

            if ((i + 1) % 16 == 0) {
                f_output << std::dec << " |";
                for (size_t j = i - 15; j < i + 1; j++) {
                    if (buffer[j] < 32 || buffer[j] > 126) {
                        f_output << ".";
                    }
                    else {
                        f_output << buffer[j];
                    }
                    if ((j + 1) % 16 == 0) {
                        f_output << std::dec << "|\n";
                    }
                }
            }
        }

        if (s % 16 != 0) {
            for (size_t i = 0; i < 16 - s % 16; i++) {
                f_output << "   ";
                // adding another blank space after 8th byte
                if ((s + i) % 8 == 0) {
                    f_output << " ";
                }
            }
            f_output << std::dec << " |";
            for (size_t i = s - s % 16; i < s; i++) {
                if (buffer[i] < 32 || buffer[i] > 126) {
                    f_output << ".";
                }
                else {
                    f_output << buffer[i];
                }
                if (i == s - 1) {
                    f_output << std::dec << "|\n";
                }
            }
        }
    }

    f_output << std::dec << std::endl;

    f_input.close();
    f_output.close();
}
