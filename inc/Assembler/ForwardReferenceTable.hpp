#pragma once

#include <list>
#include <map>
#include <string>

#include "../Elf32.hpp"

class Elf32File;

class ForwardReferenceTable {
public:

    ForwardReferenceTable();

    void add_reference(Elf32_Sym* symbol_entry, Elf32_Addr address);

    void backpatch();

    ~ForwardReferenceTable() = default;

private:

    struct SymbolReference {
        Elf32_Addr
            address;  // Address of the section that needs to be replaced with the symbol value.
        Elf32_Half
            section_index;  // Index of the section that needs to be replaced with the symbol value.
    };

    ForwardReferenceTable(const ForwardReferenceTable&) = delete;

    ForwardReferenceTable(ForwardReferenceTable&&) = delete;

    ForwardReferenceTable& operator=(const ForwardReferenceTable&) = delete;

    ForwardReferenceTable& operator=(ForwardReferenceTable&&) = delete;

    void resolve_symbol(Elf32_Sym* symbol_entry, const SymbolReference& address);

    std::map<std::string, std::list<SymbolReference>> _forward_references;
};