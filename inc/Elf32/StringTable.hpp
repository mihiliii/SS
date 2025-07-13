#pragma once

#include <fstream>
#include <map>
#include <vector>

#include "Elf32.hpp"
#include "Section.hpp"

class Elf32File;

class StringTable : public Section {
public:

    StringTable(Elf32File& elf32_file);

    const Elf32_Off add_string(const std::string& string);

    const Elf32_Off get_offset(const std::string& string);

    const std::string& get_string(Elf32_Off offset);

    void replace(const std::vector<char>& str_table_data); // NOTE: check if needed

    void write(std::ofstream* file);

    ~StringTable() = default;

private:

    std::map<Elf32_Off, std::string> _string_table;
};
