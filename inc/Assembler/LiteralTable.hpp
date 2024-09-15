#pragma once

#include <list>
#include <map>
#include <vector>

#include "../Elf32.hpp"

class CustomSection;
class Elf32File;

class LiteralTable {
public:

    LiteralTable(Elf32File* _elf32_file, CustomSection* _parent_section);

    void addLiteralReference(int _literal, Elf32_Addr _address);
    void addRelocatableSymbolReference(Elf32_Sym* _symbol_entry, Elf32_Addr _address);

    uint32_t* get(uint32_t _index) { return &literal_pool[_index]; };
    size_t size() const { return literal_pool.size(); };

    void write(std::ofstream* _file);

    void resolveReferences();

    ~LiteralTable() = default;

private:

    Elf32File* elf32_file;
    CustomSection* parent_section;

    // literal -> offset in literal pool, list of addresses in section where literal is used
    std::map<int, std::pair<Elf32_Off, std::list<Elf32_Addr>>> literal_table;
    std::map<Elf32_Sym*, std::pair<Elf32_Off, std::list<Elf32_Addr>>> symbol_value_table;
    std::vector<uint32_t> literal_pool;
};
