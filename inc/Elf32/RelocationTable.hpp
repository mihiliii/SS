#pragma once

#include <vector>

#include "Elf32.hpp"
#include "Section.hpp"

class CustomSection;

class RelocationTable : public Section {
public:

    static const std::string NAME_PREFIX;

    RelocationTable(Elf32File* _elf32_file, CustomSection* _linked_section);
    RelocationTable(Elf32File* _elf32_file, CustomSection* _linked_section, Elf32_Shdr _section_header,
                    const std::vector<Elf32_Rela>& _relocation_table);

    RelocationTable(const RelocationTable&) = delete;
    RelocationTable& operator=(const RelocationTable&) = delete;
    RelocationTable(RelocationTable&&) = delete;

    void add(Elf32_Rela _rela_entry);
    void add(Elf32_Addr _offset, Elf32_Word _info, Elf32_SWord _addend);
    void add(const std::vector<Elf32_Rela>& _relocation_table);

    void print(std::ostream& _file) const;
    void write(std::ofstream* _file) override;

    std::vector<Elf32_Rela>& relocationTable() { return relocation_table; };
    CustomSection& linkedSection() { return *linked_section; };

private:

    CustomSection* linked_section;
    std::vector<Elf32_Rela> relocation_table;
};
