#pragma once

#include <iostream>
#include <string>

#include "Elf32.hpp"

class Elf32_File;

class Section {
public:

    Elf32_Shdr& getHeader();
    Elf32_Half getIndex() const;
    std::string getName() const;

    virtual void write(std::ofstream* _file) {};

    virtual ~Section() {};

protected:

    Section(Elf32_File* _elf32_file);
    Section(Elf32_File* _elf32_file, Elf32_Shdr _section_header);

    Elf32_File* elf32_file;
    Elf32_Shdr section_header;
    uint32_t sh_table_index;
};
