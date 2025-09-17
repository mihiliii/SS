#pragma once

#include "Elf32.hpp"

#include <string>

struct Elf32File;

class Section {
public:

    friend struct Elf32File;

    virtual ~Section() = 0;

    virtual void write(std::ostream& ostream) = 0;

    virtual void print(std::ostream& ostream) const = 0;

    Elf32_Shdr get_header() const;

    Elf32_Word get_header_index() const;

    const std::string& get_name() const;

    void set_header(Elf32_Shdr header);

protected:

    Section(Elf32File& elf32_file);

    Section(Elf32File& elf32_file, Elf32_Shdr setcion_header);

    Section(const Section&) = delete;

    Section(Section&&);

    Section& operator=(const Section&) = delete;

    Section& operator=(Section&&);

    Elf32File* _elf32_file;
    Elf32_Shdr _header;
    Elf32_Word _header_index;
};
