#include "../inc/ParseElf32.hpp"

ParseElf32::ParseElf32() : elf32_file(nullptr) {}

ParseElf32::ParseElf32(std::ifstream* _file, const char* _file_name)
    : elf32_file(), elf32_header(), section_header_table(), symbol_table(), string_table() {
    elf32_file = _file;
    elf32_file->open(_file_name, std::ios::binary);
    if (!elf32_file->is_open())
        std::cerr << "Error: Could not open file.";

    // Sets elf32_header
    Elf32_Ehdr buffer;
    elf32_file->read((char*) (&buffer), sizeof(Elf32_Ehdr));
    elf32_header = Elf32Header(buffer);

    

    // Sets section_header_table
    elf32_file->seekg(elf32_header.e_shoff);

    
}

ParseElf32::ParseElf32(std::ifstream* _file) {
    elf32_file = _file;
    if (!elf32_file->is_open())
        std::cerr << "Error: File is not open.";
}

void ParseElf32::file(std::ifstream* _file, const char* _file_name) {
    elf32_file = _file;
    elf32_file->open(_file_name, std::ios::binary);
    if (!elf32_file->is_open())
        std::cerr << "Error: Could not open file.";
}

void ParseElf32::setFile(std::ifstream* _file) {
    if (!elf32_file->is_open())
        std::cerr << "Error: File is not open.";
}

Elf32_Ehdr ParseElf32::readElfHeader() {
    Elf32_Ehdr elf_header;
    elf32_file->read((char*) (&elf_header), sizeof(Elf32_Ehdr));
    return elf_header;
}

std::vector<Elf32_Shdr> ParseElf32::readSectionHeaderTable() {
    std::vector<Elf32_Shdr> section_header_table;

    Elf32_Ehdr _elf_header = readElfHeader();
    elf32_file->seekg(_elf_header.e_shoff);
    for (int i = 0; i < _elf_header.e_shnum; i++) {
        Elf32_Shdr section_header;
        elf32_file->read((char*) (&section_header), sizeof(Elf32_Shdr));
        section_header_table.push_back(section_header);
    }
    return section_header_table;
}
