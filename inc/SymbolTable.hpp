#ifndef SymbolTable_hpp_
#define SymbolTable_hpp_

#include <iostream>

#include "Section.hpp"
#include "Elf32.hpp"

class SymbolTable: public Section<Elf32_Sym> {
public:

    static SymbolTable& getInstance();

    void appendContent(Elf32_Sym* _content, size_t _size);

    SymbolTable(const SymbolTable&) = delete;
    SymbolTable& operator=(const SymbolTable&) = delete;

    ~SymbolTable() = default; 

private:

    SymbolTable(); 

};


#endif
