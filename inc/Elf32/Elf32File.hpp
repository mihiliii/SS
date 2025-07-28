#pragma once

#include <map>
#include <vector>

#include "CustomSection.hpp"
#include "Elf32.hpp"
#include "RelocationTable.hpp"
#include "StringTable.hpp"
#include "SymbolTable.hpp"

typedef Elf32_Ehdr Elf32Header;
typedef std::vector<Elf32_Shdr> SectionHeaderTable;
typedef std::map<std::string, CustomSection> CustomSectionMap;
typedef std::map<std::string, RelocationTable> RelocationTableMap;

struct Elf32File {
public:

    Elf32File();

    Elf32File(const std::string& file);

    ~Elf32File() = default;

    void write_bin(const std::string& file_name, Elf32_Half type);

    void write_hex(const std::string& file_name);

    void read_elf(const std::string& file_name);

    void read(const std::string& file);

    CustomSection* new_custom_section(const std::string& name);

    CustomSection* new_custom_section(const std::string& name, Elf32_Shdr section_header,
                                      const std::vector<char>& data);

    RelocationTable* new_relocation_table(const std::string& name, CustomSection& linked_section);

    RelocationTable* new_relocation_table(const std::string& name, CustomSection& linked_section,
                                          Elf32_Shdr section_header,
                                          const std::vector<Elf32_Rela>& data);

    Elf32Header _elf32_header;
    SectionHeaderTable _section_header_table;
    StringTable _string_table;
    SymbolTable _symbol_table;
    CustomSectionMap _custom_section_map;
    RelocationTableMap _rela_table_map;
};
