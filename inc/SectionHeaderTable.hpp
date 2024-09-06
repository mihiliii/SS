#pragma once

#include <fstream>
#include <map>
#include <vector>

#include "Elf32.hpp"

class StringTable;

class SectionHeaderTable {
public:

    static SectionHeaderTable& getInstance();

    void write(std::ofstream* _file);

    void print(std::ofstream& _file);

    uint32_t insert(Elf32_Shdr* _section_entry);

    std::map<uint32_t, Elf32_Shdr*> getSectionHeaderTable() { return section_header_table; }

    Elf32_Shdr* getSectionHeader(uint32_t _index) { return section_header_table[_index]; }

    Elf32_Shdr* getSectionHeader(std::string _section_name);

    size_t getSize() const { return section_header_table.size() * sizeof(Elf32_Shdr); }

    SectionHeaderTable(const SectionHeaderTable&) = delete;
    SectionHeaderTable& operator=(const SectionHeaderTable&) = delete;

    ~SectionHeaderTable() = default;

private:

    SectionHeaderTable();

    std::map<uint32_t, Elf32_Shdr*> section_header_table;
    static uint32_t section_header_table_index;
};
