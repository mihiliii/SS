#pragma once

#include <fstream>

#include "Elf32.hpp"

enum struct Elf32_Ehdr_Field {
    e_type,       // File type
    e_entry,      // Entry point address (virtual address where the program starts)
    e_phoff,      // Program header table file offset
    e_shoff,      // Section header table file offset
    e_phentsize,  // Program header table entry size
    e_phnum,      // Program header table entry count
    e_shentsize,  // Section header table entry size
    e_shnum,      // Section header table entry count
    e_shstrndx,   // Section header string table index entry
};

class ElfHeader {
public:

    ElfHeader();

    Elf32_Ehdr& getElfHeader() { return elf_header; }

    void setField(Elf32_Ehdr_Field _field, uint32_t _value);

    void write(std::ofstream* _file);

    void print(std::ofstream& _file);

    ElfHeader(const ElfHeader&) = delete;
    ElfHeader& operator=(const ElfHeader&) = delete;

    ~ElfHeader() = default;

private:

    Elf32_Ehdr elf_header;
};
