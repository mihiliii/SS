#pragma once

#include <list>
#include <map>
#include <vector>

#include "Elf32.hpp"

class CustomSection;

class LiteralTable {
public:

    LiteralTable(CustomSection* _parent_section) : parent_section(_parent_section) {};

    ~LiteralTable() = default;

    bool isEmpty();

    void addLiteralReference(int _literal, Elf32_Addr _address);

    Elf32_Off getLiteralOffset(int _literal);

    void printTable() const;

    void writePool(std::ofstream* _file);

    void resolveLiteralReferences();

private:

    CustomSection* parent_section;

    // literal -> offset in literal pool, list of addresses in section where literal is used
    std::map<int, std::pair<Elf32_Off, std::list<Elf32_Addr>>> literal_table;
    std::vector<uint32_t> literal_pool;
};
