#pragma once

#include <vector>

#include "Elf32.hpp"
#include "Section.hpp"

class CustomSection;
class StringTable;
class SymbolTable;

class RelocationTable : public Section {
public:

    static const std::string Rela_Name_Prefix;

    friend class Elf32File;

    ~RelocationTable() = default;

    const std::string& get_name() const;

    CustomSection& get_linked_section();

    void add_entry(Elf32_Rela rela_entry);

    void add_entry(Elf32_Addr offset, Elf32_Word info, Elf32_SWord addend);

    Elf32_Rela* get_entry(size_t index);

    Elf32_Rela* get_entry(const std::string& symbol_name);

    void print(std::ostream& file) const;

    void write(std::ofstream& file) override;

private:

    RelocationTable(Elf32File& elf32_file, CustomSection& linked_section);

    RelocationTable(Elf32File& elf32_file, CustomSection& linked_section, Elf32_Shdr section_header,
                    const std::vector<Elf32_Rela>& relocation_table);

    RelocationTable(const RelocationTable&) = delete;

    RelocationTable(RelocationTable&&) = delete;

    RelocationTable& operator=(const RelocationTable&) = delete;

    RelocationTable& operator=(RelocationTable&&) = delete;

    void replace_data(const std::vector<Elf32_Rela>& relocation_table);

    CustomSection& _linked_section;
    StringTable& _string_table;
    SymbolTable& _symbol_table;
    std::vector<Elf32_Rela> _relocation_table;
};
