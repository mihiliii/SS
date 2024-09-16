#pragma once

#include <vector>

#include "Elf32.hpp"
#include "Section.hpp"

class CustomSection;

class RelocationTable : public Section {
public:

    RelocationTable(
        Elf32File* _elf32_file,
        CustomSection* _linked_section,
        Elf32_Shdr _section_header,
        std::vector<Elf32_Rela> _relocation_table
    );
    RelocationTable(Elf32File* _elf32_file, CustomSection* _linked_section);

    void print(std::ofstream& _file) const;
    void write(std::ofstream* _file) override;

    void add(Elf32_Rela _rela_entry);
    void add(Elf32_Addr _offset, Elf32_Word _info, Elf32_SWord _addend);

    std::vector<Elf32_Rela>& getContent() { return relocation_table; };

    bool isEmpty() const { return relocation_table.empty(); };

private:

    CustomSection* parent_section;
    std::vector<Elf32_Rela> relocation_table;
};
