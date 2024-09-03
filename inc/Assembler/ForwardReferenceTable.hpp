#pragma once

#include <map>

#include "SymbolTable.hpp"


class ForwardReferenceTable {
public:

    void add(Elf32_Sym* _symbol_entry, Elf32_Addr _address);

    void backpatch();

private:

    struct symbol_reference {
        Elf32_Addr address;        // Address of the section that needs to be replaced with the symbol value.
        Elf32_Half section_index;  // Index of the section that needs to be replaced with the symbol value.
    };

    void resolveSymbol(Elf32_Sym* _symbol_entry, symbol_reference& _address);

    std::map<std::string, std::list<symbol_reference>> forward_references;
};