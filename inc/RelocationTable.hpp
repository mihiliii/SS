#pragma once

#include "Elf32.hpp"
#include "Section.hpp"
#include <vector>

class CustomSection;

class RelocationTable : public Section {
public:

    RelocationTable(CustomSection* _linked_section);

    void print() const;

    void write(std::ofstream* _file) override;

    void add(Elf32_Rela _rela_entry);

    void add(Elf32_Addr _offset, Elf32_Word _info, Elf32_SWord _addend);

private:

    CustomSection* parent_section;

    std::vector<Elf32_Rela> relocation_table;
};
