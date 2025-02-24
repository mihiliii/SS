#pragma once

#include <fstream>

#include "Elf32.hpp"

class Elf32File;

class Section {
public:

    friend class Elf32File;

    virtual ~Section() = 0;

    Elf32_Word get_index() const;

    Elf32_Shdr get_header() const;

    void set_header(const Elf32_Shdr& header);

    virtual void write(std::ofstream& file) = 0;

protected:

    Section(Elf32File& elf32_file);

    Section(Elf32File& elf32_file, Elf32_Shdr& section_header);

    Elf32File& _elf32_file;
    Elf32_Word _header_index;
    Elf32_Shdr& _header;

private:

    Section(const Elf32File&) = delete;

    Section(Elf32File&&) = delete;

    Section& operator=(const Elf32File&) = delete;

    Section& operator=(Elf32File&&) = delete;
};
