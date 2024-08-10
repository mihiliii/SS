#pragma once

#include "Elf32.hpp"
#include <fstream>

class ElfHeader {
public:

    static ElfHeader& getInstance();

    Elf32_Ehdr& getElfHeader() { return elf_header; }

    void write(std::ofstream* _file);

    ElfHeader(const ElfHeader&) = delete;
    ElfHeader& operator=(const ElfHeader&) = delete;

    ~ElfHeader() = default;

private:

    Elf32_Ehdr elf_header;

    ElfHeader() = default;

};