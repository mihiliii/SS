#pragma once

#include <fstream>
#include <vector>

#include "Elf32.hpp"
#include "Section.hpp"

class StringTable : public Section {
public:

    static StringTable& getInstance();

    Elf32_Off addString(std::string _string);

    std::string getString(Elf32_Off _offset);

    Elf32_Off findString(std::string _string);

    void write(std::ofstream* _file) override;

    StringTable(const StringTable&) = delete;
    StringTable& operator=(const StringTable&) = delete;

    ~StringTable() = default;

private:

    StringTable();

    std::map<Elf32_Off, std::string> string_table;
};
