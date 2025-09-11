#pragma once

#include <vector>

#include "Section.hpp"

class CustomSection;

class RelocationTable : public Section {
public:

    static const std::string NAME_PREFIX;

    static std::string get_rela_name(const std::string& custom_section_name);

    static std::string get_rela_name(CustomSection& linked_section);

    static std::string get_custom_section_name(const std::string& relocation_name);

    static std::string get_custom_section_name(RelocationTable& relocation_table);

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

    std::vector<Elf32_Rela>& get_relocation_table();

    CustomSection& get_linked_section();

    void write(std::ostream& ostream) override;

    void print(std::ostream& ostream) const override;

private:

    CustomSection& _linked_section;
    std::vector<Elf32_Rela> _relocation_table;
};
