#pragma once

#include <iostream>
#include <map>
#include <string>

#include "Elf32.hpp"

class Section {
public:

    virtual ~Section() = 0;

    virtual void write(std::ofstream* _file) = 0;

    Elf32_Shdr& getHeader() { return section_header; };

    uint32_t getSectionHeaderTableIndex() const { return section_header_table_index; };

    std::string getName() const;

    static std::string getName(uint32_t _index);

protected:

    Section();

    Section(std::string _name);

    Elf32_Shdr section_header;
    uint32_t section_header_table_index;
};
