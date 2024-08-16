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

    void addSymbol(std::string _name, Elf32_Addr _value, bool _defined);

    void setInfo(std::string _name, Elf32_Half _info);

    void setInfo(Elf32_Sym* _symbol, Elf32_Half _info);

    Elf32_Sym* findSymbol(std::string _name);

    void printContent() const override;

    void write(std::ofstream* _file) override;

private:

    SymbolTable();

    std::vector<Elf32_Sym> content;
};
