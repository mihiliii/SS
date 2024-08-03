#pragma once


#include <iostream>

#include "Section.hpp"
#include "Elf32.hpp"

class SymbolTable: public Section<Elf32_Sym> {
public:

    static SymbolTable& getInstance();

    void appendContent(Elf32_Sym* _content, size_t _size);

    void addSymbol(Section* _section, const std::string& _name, Elf32_Addr _value, Elf32_Word _size);

    SymbolTable(const SymbolTable&) = delete;
    SymbolTable& operator=(const SymbolTable&) = delete;

    ~SymbolTable() = default; 

private:

    SymbolTable(); 

};
