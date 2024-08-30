#pragma once

#include <iostream>
#include <map>
#include <string>

#include "Elf32.hpp"

class Section {
public:

    friend class SectionHeaderTable;

    virtual ~Section() {};

    virtual void write(std::ofstream* _file) {};

    Elf32_Shdr& getHeader() { return section_header; };

    Elf32_Half getSectionHeaderTableIndex() const { return section_header_table_index; };

    std::string getName() const;

    static std::string getName(uint32_t _index);

protected:

    Section();

    Section(std::string _name);

    Elf32_Shdr section_header;
    Elf32_Half section_header_table_index;
};
