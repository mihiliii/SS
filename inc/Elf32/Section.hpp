#pragma once

#include <string>

#include "Elf32.hpp"

class Elf32File;

class Section {
public:

    Elf32_Shdr& get_header();

    const Elf32_Word get_index() const;

    const std::string& name() const;

    virtual void write(std::ofstream* _file) {};

    virtual ~Section() = 0;

protected:

    Section(Elf32File& elf32_file);
    Section(Elf32File& elf32_file, Elf32_Shdr setcion_header);

    Elf32File& _elf32_file;
    Elf32_Shdr _header;
    Elf32_Word _header_index;
};
