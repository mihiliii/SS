#pragma once

#include "Elf32.hpp"
#include "Section.hpp"
#include "CustomSection.hpp"
#include <vector>

class RelocationTable : public Section {
public:

    RelocationTable(CustomSection* _linked_section);

    void print() const;

    void write(std::ofstream* _file) override;

    void addRelocation(Elf32_Rela _rela_entry);

private:

    std::vector<Elf32_Rela> relocation_table;
};
