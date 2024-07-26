#include "../inc/SymbolTable.hpp"

#include "../inc/Elf32.hpp"
#include "../inc/SectionHeaderStringTable.hpp"
#include "../inc/Section.hpp"

SymbolTable& SymbolTable::getInstance() {
    static SymbolTable instance;
    return instance;
}

SymbolTable::SymbolTable() : Section("symtab") {}
