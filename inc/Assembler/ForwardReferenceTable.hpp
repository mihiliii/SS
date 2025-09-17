#pragma once

#include "Elf32/Elf32File.hpp"

#include <list>
#include <unordered_map>

class Assembler;

class ForwardReferenceTable {
public:

    ForwardReferenceTable(Assembler& assembler);

    ForwardReferenceTable(const ForwardReferenceTable&) = delete;
    ForwardReferenceTable(ForwardReferenceTable&&) = default;

    ForwardReferenceTable& operator=(const ForwardReferenceTable&) = delete;
    ForwardReferenceTable& operator=(ForwardReferenceTable&&) = delete;

    void add_reference(Elf32_Sym& symbol_entry, Elf32_Addr address);

    void backpatch();

    ~ForwardReferenceTable() = default;

private:

    struct SymbolReference {
        Elf32_Addr address;
        CustomSection* section;
    };

    void resolve_symbol(Elf32_Sym& symbol_entry, SymbolReference& address);

    Assembler& _assembler;
    Elf32File& _elf32_file;
    std::unordered_map<Elf32_Sym*, std::list<SymbolReference>> forward_references;
};
