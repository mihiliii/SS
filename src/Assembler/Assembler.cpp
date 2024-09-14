#include "../../inc/Assembler/Assembler.hpp"

#include <cstddef>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "../../inc/Assembler/ForwardReferenceTable.hpp"
#include "../../inc/Assembler/LiteralTable.hpp"
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
    elf32_file = new Elf32File(_output_file_name, true);

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

    return 0;
}

void Assembler::startBackpatching() {
    forward_reference_table.backpatch();

    for (auto iterator : elf32_file->getCustomSections()) {
        current_section = iterator.second;
        current_section->getLiteralTable()->resolveReferences();
    }
}

int Assembler::writeToBinFile(const char* _output_file_name) {
    std::ofstream f_output(_output_file_name, std::ios::out | std::ios::binary);

    if (!f_output.is_open()) {
        std::cerr << "Error: can't open file " << std::string(_output_file_name) << std::endl;
        return -1;
    }

    // Write sections right after the ELF header:
    f_output.seekp(sizeof(Elf32_Ehdr), std::ios::beg);

    for (auto iterator : elf32_file->getCustomSections()) {
        iterator.second->write(&f_output);
    }

    // Write the string table:
    elf32_file->getStringTable().write(&f_output);

    // Write the symbol table:
    elf32_file->getSymbolTable().write(&f_output);

    // Set section header table offset and number of entries in the ELF header:
    std::streampos section_header_table_offset = f_output.tellp();
    elf32_file->getElf32Header().e_type = ET_REL;
    elf32_file->getElf32Header().e_shoff = section_header_table_offset;
    elf32_file->getElf32Header().e_shnum = elf32_file->getSectionHeaderTable().size();

    // Write the section header table:
    for (Elf32_Shdr* section_header : elf32_file->getSectionHeaderTable()) {
        f_output.write((char*) section_header, sizeof(Elf32_Shdr));
    }

    // Write the ELF header at the beginning of the file:
    f_output.seekp(0, std::ios::beg);
    f_output.write((char*) &elf32_file->getElf32Header(), sizeof(Elf32_Ehdr));

    f_output.close();

    return 0;
}

int Assembler::writeToTxtFile(const char* _input_file_name) {
    std::ifstream f_input(_input_file_name, std::ios::in | std::ios::binary);
    std::ofstream f_output("readelf.txt", std::ios::out);

    if (!f_input.is_open()) {
        std::cout << "Error: can't open " << _input_file_name << std::endl;
        return -1;
    }
    else if (!f_output.is_open()) {
        std::cout << "Error: can't open readelf.txt" << std::endl;
        return -1;
    }

    size_t bufferSize = 1024;
    std::vector<char> buffer(bufferSize);

    int len = (90 - (std::string(_input_file_name).length() + 9)) / 2;

    f_output << std::setw(len) << std::setfill('*') << "   " << "ELFREAD: " << _input_file_name << std::setw(len)
             << std::setfill('*') << std::left << "   " << std::endl;

    f_output << std::endl << "Elf Header:" << std::endl;
    f_output << "  Identification: ";
    for (int i = 0; i < EI_NIDENT; i++) {
        f_output << (elf32_file->getElf32Header().e_ident[i]) << " ";
    }
    f_output << std::hex << std::endl;
    switch (elf32_file->getElf32Header().e_type) {
        case ET_NONE:
            f_output << "  Type: No file type" << std::endl;
            break;
        case ET_REL:
            f_output << "  Type: Relocatable file" << std::endl;
            break;
        case ET_EXEC:
            f_output << "  Type: Executable file" << std::endl;
            break;
        case ET_DYN:
            f_output << "  Type: Shared object file" << std::endl;
            break;
        default:
            break;
    }
    f_output << "  Entry point address: 0x" << elf32_file->getElf32Header().e_entry << std::endl;
    f_output << "  Section header offset: 0x" << elf32_file->getElf32Header().e_shoff << std::endl;
    f_output << std::dec;
    f_output << "  Section header entry size: " << elf32_file->getElf32Header().e_shentsize << " (bytes)" << std::endl;
    f_output << "  Number of section headers: " << elf32_file->getElf32Header().e_shnum << std::endl;
    f_output << std::hex;
    f_output << "  Page header offset: 0x" << elf32_file->getElf32Header().e_phoff << std::endl;
    f_output << std::dec;
    f_output << "  Page header entry size: " << elf32_file->getElf32Header().e_phentsize << " (bytes)" << std::endl;
    f_output << "  Number of page headers: " << elf32_file->getElf32Header().e_phnum << std::endl;

    f_output << std::endl << "Section Header Table:" << std::endl;
    f_output << "  ";
    f_output << std::left << std::setfill(' ');
    f_output << std::setw(4) << "NUM";
    f_output << std::setw(25) << "NAME";
    f_output << std::setw(5) << "TYPE";
    f_output << std::setw(9) << "ADDRESS";
    f_output << std::setw(9) << "OFFSET";
    f_output << std::setw(9) << "SIZE";
    f_output << std::setw(9) << "FLAGS";
    f_output << std::setw(5) << "LINK";
    f_output << std::setw(5) << "INFO";
    f_output << std::setw(6) << "ALIGN";
    f_output << std::setw(9) << "ENTSIZE";
    f_output << std::endl;
    uint32_t index = 0;
    for (auto& iterator : elf32_file->getSectionHeaderTable()) {
        Elf32_Shdr* section = iterator;
        f_output << "  ";
        f_output << std::setw(3) << std::right << std::dec << std::setfill(' ') << index << " ";
        f_output << std::left;
        f_output << std::setw(24) << elf32_file->getStringTable().get(section->sh_name) << " ";
        f_output << std::right << std::hex << std::setfill('0');
        f_output << std::setw(4) << section->sh_type << " ";
        f_output << std::setw(8) << section->sh_addr << " ";
        f_output << std::setw(8) << section->sh_offset << " ";
        f_output << std::setw(8) << section->sh_size << " ";
        f_output << std::setw(8) << section->sh_flags << " ";
        f_output << std::setw(4) << section->sh_link << " ";
        f_output << std::setw(4) << section->sh_info << " ";
        f_output << std::setw(5) << std::dec << std::setfill(' ') << std::left << section->sh_addralign << " ";
        f_output << std::setw(8) << section->sh_entsize << std::endl;
        index++;
    }

    for (auto iterator : elf32_file->getCustomSections()) {
        RelocationTable* relocation_table = iterator.second->getRelocationTable();
        if (!relocation_table->isEmpty()) {
            relocation_table->print(f_output);
        }
    }

    elf32_file->getSymbolTable().print(f_output);

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

    return 0;
}
