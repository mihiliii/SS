#pragma once

#include <fstream>
#include <vector>
#include <map>

#include "Elf32.hpp"

class SectionHeaderTable {
public:

    SectionHeaderTable();

    void write(std::ofstream* _file);

    void print();

    uint32_t insert(Elf32_Shdr* _section_entry);

    Elf32_Shdr* getSectionHeader(uint32_t _index) { return section_header_table[_index]; }

    size_t getSize() const { return section_header_table.size() * sizeof(Elf32_Shdr); }

    SectionHeaderTable(const SectionHeaderTable&) = delete;
    SectionHeaderTable& operator=(const SectionHeaderTable&) = delete;

    ~SectionHeaderTable() = default;

private:

    std::map<uint32_t, Elf32_Shdr*> section_header_table;
    static uint32_t section_header_table_index;

};

