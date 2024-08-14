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

    virtual void printContent() const = 0;

    virtual void write(std::ofstream* _file) = 0;

    Elf32_Shdr& getSectionHeader() { return section_header; };

    uint32_t getSectionHeaderTableIndex() const { return section_header_table_index; };

    static std::map<std::string, Section*>& getSectionTable() { return section_table; };

protected:

    Section(std::string _name);

    Elf32_Shdr section_header;
    std::string name;
    uint32_t section_header_table_index;

    static std::map<std::string, Section*> section_table;
};
