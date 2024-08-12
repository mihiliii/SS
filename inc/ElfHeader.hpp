#pragma once

#include <fstream>

#include "Elf32.hpp"

enum ElfHeaderField {
    SH_OFFSET,
    SH_ENTRYSIZE,
    SH_NUM,
};

class ElfHeader {
public:

    static ElfHeader& getInstance();

    ElfHeader(const ElfHeader&) = delete;
    ElfHeader& operator=(const ElfHeader&) = delete;

    ~ElfHeader() = default;

    Elf32_Ehdr& getElfHeader() { return elf_header; }

    void setField(ElfHeaderField _field, uint32_t _value);

    void write(std::ofstream* _file);

private:

    Elf32_Ehdr elf_header;

    ElfHeader();
};
