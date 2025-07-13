#pragma once

#include <vector>

#include "Elf32.hpp"
#include "Section.hpp"

class CustomSection;

class RelocationTable : public Section {
public:

    static const std::string NAME_PREFIX;

    RelocationTable(Elf32File& elf32_file, CustomSection& linked_section);
    RelocationTable(Elf32File& elf32_file, CustomSection& linked_section, Elf32_Shdr section_header,
                    const std::vector<Elf32_Rela>& relocation_table);

    RelocationTable(const RelocationTable&) = delete;
    RelocationTable& operator=(const RelocationTable&) = delete;
    RelocationTable(RelocationTable&&) = delete;

    void add_entry(Elf32_Rela rela_entry);
    void add_entry(Elf32_Addr offset, Elf32_Word info, Elf32_SWord addend);
    void add_entry(const std::vector<Elf32_Rela>& relocation_table);

    std::vector<Elf32_Rela>& relocation_table() { return _relocation_table; };
    CustomSection& linked_section() { return *_linked_section; };

    void print(std::ostream& file) const;

    void write(std::ofstream* file) override;

private:

    // TODO: remove * if possible
    CustomSection& _linked_section;
    std::vector<Elf32_Rela> _relocation_table;
};
