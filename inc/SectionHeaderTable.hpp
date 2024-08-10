#pragma once

#include <vector>

#include "Elf32.hpp"
#include <fstream>

class SectionHeaderTable {
public:

    static SectionHeaderTable& getInstance();

    uint32_t insert(Elf32_Shdr* _section_entry);

    void write(std::ofstream* _file);

    void printSectionHeaderTable();

    size_t getSize() const { return section_header_table.size(); }

    SectionHeaderTable(const SectionHeaderTable&) = delete;
    SectionHeaderTable& operator=(const SectionHeaderTable&) = delete;

    ~SectionHeaderTable() = default;

private:

    SectionHeaderTable();

    std::vector<Elf32_Shdr*> section_header_table;
    uint32_t section_header_table_index;
};

