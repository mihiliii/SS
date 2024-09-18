#pragma once

#include <fstream>
#include <vector>
#include <map>

#include "Elf32.hpp"
#include "Section.hpp"

class Elf32File;

class StringTable : public Section {
public:

    StringTable(Elf32File* _elf32_file);

    Elf32_Off add(std::string _string);

    Elf32_Off get(std::string _string);
    std::string get(Elf32_Off _offset);

    void replace(std::vector<char> _str_table_data);

    void write(std::ofstream* _file);

    ~StringTable() = default;

private:

    std::map<Elf32_Off, std::string> string_table;
};
