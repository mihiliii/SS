#pragma once

#include "Elf32.hpp"
#include "Section.hpp"

class StringTable : public Section {
public:

    Elf32_Off addString(std::string _symbol);

    void printContentHex() const;

    void printContent() const;

    static StringTable& getInstance();

    void write(std::ofstream* _file);

    StringTable(const StringTable&) = delete;
    StringTable& operator=(const StringTable&) = delete;

    ~StringTable() = default;

private:

    StringTable();

    std::vector<char> content;
};
