#pragma once

#include "Section.hpp"

#include <map>
#include <vector>

class StringTable : public Section {
public:

    StringTable(Elf32File& elf32_file);

    StringTable(const StringTable&) = delete;

    StringTable(StringTable&&) = default;

    StringTable& operator=(const StringTable&) = delete;

    StringTable& operator=(StringTable&&) = default;

    ~StringTable() = default;

    const Elf32_Off add_string(const std::string& string);

    Elf32_Off get_offset(const std::string& string) const;

    const std::string& get_string(Elf32_Off offset) const;

    void set_string_table(const std::vector<char>& str_table_data);

    void write(std::ostream& ostream) override;

    void print(std::ostream& ostream) const override;

private:

    std::map<Elf32_Off, std::string> _string_table;
};
