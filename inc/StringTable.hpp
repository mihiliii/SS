#pragma once

#include "Elf32.hpp"
#include "Section.hpp"

class StringTable : public Section {
public:

    static StringTable& getInstance();

    StringTable(const StringTable&) = delete;
    StringTable& operator=(const StringTable&) = delete;

    ~StringTable() = default;

    void addString(std::string _symbol, Elf32_Off* _offset);

    void printContent() const override;

    void write(std::ofstream* _file) override;

private:

    StringTable();

    std::vector<char> content;
};
