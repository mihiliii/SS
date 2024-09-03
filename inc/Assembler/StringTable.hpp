#pragma once

#include "Elf32.hpp"
#include "Section.hpp"
#include <vector>
#include <fstream>

class StringTable : public Section {
public:

    StringTable();

    Elf32_Off addString(std::string _string);

    std::string getString(Elf32_Off _offset);

    Elf32_Off findString(std::string _string);

    void write(std::ofstream* _file) override;

    StringTable(const StringTable&) = delete;
    StringTable& operator=(const StringTable&) = delete;

    ~StringTable() = default;

private:

    std::map<Elf32_Off, std::string> string_table;
};
