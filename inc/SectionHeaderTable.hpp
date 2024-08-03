#pragma once


#include <vector>

#include "Elf32.hpp"

class SectionHeaderTable {
public:

    static SectionHeaderTable& getInstance(); 

    void insert(Elf32_Shdr* _section_entry);

    void printSectionHeaderTable();

    SectionHeaderTable(const SectionHeaderTable&) = delete;
    SectionHeaderTable& operator=(const SectionHeaderTable&) = delete;

    ~SectionHeaderTable() = default;

private:

    SectionHeaderTable() = default;

    std::vector<Elf32_Shdr*> section_header_table;
};

