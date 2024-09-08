#pragma once

#include <iostream>
#include <map>
#include <string>

#include "Elf32.hpp"
#include "SectionHeaderTable.hpp"

class ParseElf32;

class Section {
public:

    friend class ParseElf32;

    virtual ~Section() {};

    virtual void write(std::ofstream* _file) {};

    Elf32_Shdr* getHeader() { return sht->getSectionHeader(sht_index); };

    Elf32_Half getSectionHeaderTableIndex() const { return sht_index; };

    std::string getName() const;

protected:

    Section(SectionHeaderTable* _sht, Elf32_Shdr* _section_header = nullptr);

    Section(SectionHeaderTable* _sht, std::string _name, Elf32_Shdr* _section_header = nullptr);

    SectionHeaderTable* sht;
    Elf32_Shdr* section_header;
    Elf32_Half sht_index;
};
