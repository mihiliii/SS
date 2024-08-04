#pragma once

#include "Elf32.hpp"
#include "Section.hpp"

class SymbolStringTable : public Section {
public:

    Elf32_Half addSymbol(const std::string& _symbol);

    static SymbolStringTable& getInstance();

    SymbolStringTable(const SymbolStringTable&) = delete;
    SymbolStringTable& operator=(const SymbolStringTable&) = delete;

    ~SymbolStringTable() = default;

private:

    SymbolStringTable();

    std::vector<char> content;
};
