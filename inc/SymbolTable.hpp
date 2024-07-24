#ifndef SymbolTable_hpp_
#define SymbolTable_hpp_

#include <iostream>

#include "Section.hpp"
#include "Elf32.hpp"

class SymbolTable: public Section<Elf32_Sym> {
public:

};


#endif
