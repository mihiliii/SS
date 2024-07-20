#ifndef SymbolTable_hpp_
#define SymbolTable_hpp_

#include <iostream>

#include "Elf32.hpp"

class SymbolTable {
public:

    SymbolTable() : symbol_table_header_() {}

private:

    Elf32_Shdr symbol_table_header_;
};


#endif
