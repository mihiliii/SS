#pragma once

#include <iostream>

#include "Elf32.hpp"
#include "Section.hpp"

class SymbolTable : public Section {
public:

    static SymbolTable& getInstance();

    SymbolTable(const SymbolTable&) = delete;
    SymbolTable& operator=(const SymbolTable&) = delete;

    ~SymbolTable() = default;

    void addSymbol(Elf32_Sym* _content);

    void addSymbol(std::string _name, Elf32_Addr _value);

    void printContent() const override;

    void write(std::ofstream* _file) override;

private:

    SymbolTable();

    std::vector<Elf32_Sym> content;
};
