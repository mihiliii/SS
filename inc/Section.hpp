#pragma once

#include <iostream>
#include <string>

#include "Elf32.hpp"

class Elf32File;

class Section {
public:

    Elf32_Shdr& header();
    Elf32_Half index() const;
    std::string name() const;

    virtual void write(std::ofstream* _file) {};

    virtual ~Section() = 0;

protected:

    Section(Elf32File* _elf32_file);
    Section(Elf32File* _elf32_file, Elf32_Shdr _section_header);

    Elf32File* elf32_file;
    uint32_t sh_table_index;
};
