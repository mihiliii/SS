#pragma once

#include "Elf32.hpp"
#include "Section.hpp"
#include <vector>

class RelocationTable : public Section {
public:

    RelocationTable(InputSection* _linked_section);

    void printContent() const override;

    void write(std::ofstream* _file) override;

    void addRelocation(Elf32_Rela _rela_entry);


private:

    std::vector<Elf32_Rela> relocation_table;
};
