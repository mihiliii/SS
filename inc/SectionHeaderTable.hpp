#ifndef SectionHeaderTable_hpp_
#define SectionHeaderTable_hpp_

#include <vector>

#include "Elf32.hpp"

class SectionHeaderTable {
public:

    SectionHeaderTable(const SectionHeaderTable&) = delete;

    SectionHeaderTable& operator=(const SectionHeaderTable&) = delete;

    ~SectionHeaderTable() = default;

    static SectionHeaderTable& getInstance(); 

    void insertSectionEntry(Elf32_Shdr section_entry);

    void printSectionTable();

private:

    SectionHeaderTable() : section_header_table({}) {}

    std::vector<Elf32_Shdr> section_header_table;
};

#endif
