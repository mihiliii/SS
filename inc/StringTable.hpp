#pragma once

#include "Elf32.hpp"
#include "Section.hpp"

class StringTable : public Section {
public:

    StringTable();

    Elf32_Off addString(std::string _string);

    std::string getString(Elf32_Off _offset);

    void write(std::ofstream* _file) override;

    StringTable(const StringTable&) = delete;
    StringTable& operator=(const StringTable&) = delete;

    ~StringTable() = default;

private:

    std::vector<char> content;
};
