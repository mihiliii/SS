#pragma once

#include <iostream>

#include "Elf32.hpp"
#include "Section.hpp"

class SymbolTable : public Section {
public:

    static SymbolTable& getInstance();

    void appendContent(Elf32_Sym* _content);

    void appendContent(std::string _name, Elf32_Addr _value);

    SymbolTable(const SymbolTable&) = delete;
    SymbolTable& operator=(const SymbolTable&) = delete;

    void printContent() const;

    ~SymbolTable() = default;

private:

    SymbolTable();

    std::vector<Elf32_Sym> content;
};
