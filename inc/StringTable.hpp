#pragma once

#include <fstream>
#include <vector>

#include "Elf32.hpp"
#include "Section.hpp"

class StringTable : public Section {
public:

    // Used in linker
    StringTable(SectionHeaderTable* _sht, Elf32_Shdr* _section_header, std::vector<char> _str_table_data);

    // Used in assembler
    StringTable(SectionHeaderTable* _sht);

    // Creates empty string table
    StringTable() = default;

    Elf32_Off addString(std::string _string);

    std::string getString(Elf32_Off _offset);

    Elf32_Off findString(std::string _string);

    void write(std::ofstream* _file) override;

    ~StringTable() = default;

private:

    std::map<Elf32_Off, std::string> string_table;
};
