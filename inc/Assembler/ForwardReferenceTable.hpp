#pragma once

#include <map>
#include <list>
#include <string>

#include "../Elf32.hpp"

class ForwardReferenceTable {
public:

    ForwardReferenceTable() = default;

    void add(Elf32_Sym* _symbol_entry, Elf32_Addr _address);

    void backpatch();

    ForwardReferenceTable(const ForwardReferenceTable&) = delete;
    ForwardReferenceTable& operator=(const ForwardReferenceTable&) = delete;

    ~ForwardReferenceTable() = default;

private:

    struct SymbolReference {
        Elf32_Addr address;        // Address of the section that needs to be replaced with the symbol value.
        Elf32_Half section_index;  // Index of the section that needs to be replaced with the symbol value.
    };

    void resolveSymbol(Elf32_Sym* _symbol_entry, SymbolReference& _address);

    std::map<std::string, std::list<SymbolReference>> forward_references;
};