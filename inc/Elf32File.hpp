#pragma once

#include <map>
#include <vector>

#include "CustomSection.hpp"
#include "Elf32.hpp"
#include "RelocationTable.hpp"
#include "StringTable.hpp"
#include "SymbolTable.hpp"

typedef std::vector<Elf32_Shdr> SectionHeaderTable;
typedef Elf32_Ehdr Elf32Header;
typedef std::map<std::string, CustomSection> CustomSectionMap;
typedef std::map<CustomSection*, RelocationTable> RelocationTableMap;

class Elf32File {
public:

    static const Elf32Header Elf32_Header_Init;

    static void ReadElf(const std::string& file_name);

    enum class Elf32OutputType { OBJ, HEX };

    Elf32File();

    Elf32File(const std::string& file_name);

    ~Elf32File() = default;

    Elf32Header& get_elf32_header();

    SectionHeaderTable& get_section_header_table();

    StringTable& get_string_table();

    SymbolTable& get_symbol_table();

    CustomSectionMap& get_custom_section_map();

    RelocationTableMap& get_relocation_table_map();

    void set_type(Elf32_Half type);

    void load(const std::string& file_name);

    void save(const std::string& file_name, Elf32OutputType output);

    CustomSection& new_custom_section(const std::string& name);

    CustomSection& new_custom_section(const std::string& name, Elf32_Shdr section_header,
                                      const std::vector<char>& data);

    RelocationTable& new_relocation_table(CustomSection& linked_section);

    RelocationTable& new_relocation_table(CustomSection& linked_section, Elf32_Shdr section_header,
                                          const std::vector<Elf32_Rela>& data);

private:

    Elf32Header _elf32_header;
    SectionHeaderTable _section_header_table;
    StringTable _string_table;
    SymbolTable _symbol_table;
    CustomSectionMap _custom_section_map;
    RelocationTableMap _relocation_table_map;
};