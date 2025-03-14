#pragma once

#include <fstream>
#include <map>
#include <vector>

#include "Elf32.hpp"
#include "Section.hpp"

class StringTable : public Section {
public:

    static const Elf32_Off Null_String_Offset = 0;

    friend class Elf32File;

    ~StringTable() = default;

    const std::string& get_string(Elf32_Off offset) const;

    Elf32_Off get_string_offset(const std::string& string) const;

    Elf32_Off add_string(const std::string& string);

    void write(std::ofstream& file) override;

private:

    StringTable(Elf32File& elf32_file);

    StringTable(const Elf32File& elf32_file) = delete;

    StringTable(Elf32File&& elf32_file) = delete;

    StringTable& operator=(const Elf32File& elf32_file) = delete;

    StringTable& operator=(Elf32File&& elf32_file) = delete;

    void replace_data(const std::vector<char>& string_table_data);

    void replace_data(const std::vector<std::string>& string_table_data);

    std::map<Elf32_Off, std::string> _str_table;
};
