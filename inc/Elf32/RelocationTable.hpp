#pragma once

#include <vector>

#include "Section.hpp"

class CustomSection;

class RelocationTable : public Section {
public:

    static const std::string NAME_PREFIX;

    RelocationTable(Elf32File& elf32_file, CustomSection& linked_section);

    RelocationTable(Elf32File& elf32_file, CustomSection& linked_section, Elf32_Shdr section_header,
                    const std::vector<Elf32_Rela>& relocation_table);

    RelocationTable(const RelocationTable&) = delete;

    RelocationTable(RelocationTable&&) = delete;

    RelocationTable& operator=(const RelocationTable&) = delete;

    RelocationTable& operator=(RelocationTable&&) = delete;

    ~RelocationTable() = default;

    void add_entry(Elf32_Rela rela_entry);

    void add_entry(Elf32_Addr offset, Elf32_Word info, Elf32_SWord addend);

    void add_entry(const std::vector<Elf32_Rela>& relocation_table);

    std::vector<Elf32_Rela>& relocation_table();

    CustomSection& linked_section();

    void write(std::ostream& ostream) override;

    void print(std::ostream& ostream) const override;

private:

    CustomSection& _linked_section;
    std::vector<Elf32_Rela> _relocation_table;
};
