#pragma once

#include <fstream>
#include <vector>
#include <map>

#include "Elf32.hpp"

class Elf32_File;

class StringTable {
public:

    StringTable(Elf32_File* _elf32_file, std::vector<char> _str_table_data);
    StringTable(Elf32_File* _elf32_file);

    Elf32_Off add(std::string _string);

    Elf32_Off get(std::string _string);
    std::string get(Elf32_Off _offset);

    void write(std::ofstream* _file);

    ~StringTable() = default;

private:

    Elf32_File* elf32_file;
    std::map<Elf32_Off, std::string> string_table;
};
