#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "Elf32.hpp"
#include "SectionHeaderTable.hpp"

class Section {
public:

    virtual ~Section() = 0;

    Elf32_Shdr& getSectionHeader();

    std::string getName() const;

    uint32_t getSectionHeaderTableIndex() const;


protected:

    Section();

    Elf32_Shdr section_header;
    std::string name;
    uint32_t section_header_table_index;

    static std::map<std::string, Section*> section_table;
};
