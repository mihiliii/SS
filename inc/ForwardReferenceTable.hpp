#pragma once

#include <map>

#include "SymbolTable.hpp"

#define FRT_INDIRECT 0x0
#define FRT_DIRECT   0x1

class ForwardReferenceTable {
private:

    struct symbol_reference {
        bool direct_ref;           // If false, symbol will be put in literal pool and last 12b are replaced as offset.
        Elf32_Addr address;        // Address of the section that needs to be replaced with the symbol value.
        Elf32_Half section_index;  // Index of the section that needs to be replaced with the symbol value.
    };

public:

    void add(Elf32_Sym* _symbol_entry, Elf32_Addr _address, bool _direct_ref);

    void resolveSymbolForwardReferences();

private:

    void resolveSymbol(Elf32_Sym* _symbol_entry, symbol_reference& _address);

    std::map<std::string, std::list<symbol_reference>> forward_references;
};