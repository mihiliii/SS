#ifndef SymbolTable_hpp_
#define SymbolTable_hpp_

#include "Elf32.hpp"
#include <iostream>

class SymbolTable {
public:

    SymbolTable() : symbol_table_header_() {
    }

private:

    Elf32_Shdr symbol_table_header_;

};


#endif