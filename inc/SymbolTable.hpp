#pragma once

#include <iostream>

#include "Elf32.hpp"
#include "Section.hpp"

class SymbolTable : public Section {
public:

    static SymbolTable& getInstance();

    void appendContent(Elf32_Sym* _content, size_t _size);

    void addSymbol(
        Section* _section, const std::string& _name, Elf32_Addr _value, Elf32_Word _size
    );

    SymbolTable(const SymbolTable&) = delete;
    SymbolTable& operator=(const SymbolTable&) = delete;

    ~SymbolTable() = default;

private:

    SymbolTable();

    std::vector<Elf32_Sym> content;
};
