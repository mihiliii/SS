#pragma once

#include <list>
#include <map>
#include <vector>

#include "../Elf32.hpp"

class CustomSection;
class Elf32File;

class LiteralTable {
public:


    LiteralTable(Elf32File& elf32_file, CustomSection& linked_section);

    void add_literal_reference(int literal, Elf32_Addr address);

    void add_symbol_reference(Elf32_Sym* symbol_entry, Elf32_Addr address);

    void addLiteralPoolToSection();

    uint32_t* get_literal(size_t index);

    size_t get_pool_size() const;

    void resolve_references();

    ~LiteralTable() = default;

private:

    struct References {
        Elf32_Off pool_offset;
        std::list<Elf32_Addr> addresses;
    };

    Elf32File& _elf32_file;
    CustomSection& _linked_section;

    // literal -> offset in literal pool, list of addresses in section where literal is used
    std::map<uint32_t, References> _literal_references;
    std::map<Elf32_Sym*, References> _symbol_references;
    std::vector<uint32_t> _data_pool;
};
