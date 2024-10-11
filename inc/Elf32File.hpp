#pragma once

#include <fstream>
#include <list>
#include <map>
#include <vector>
#include <memory>

#include "CustomSection.hpp"
#include "Elf32.hpp"
#include "RelocationTable.hpp"
#include "StringTable.hpp"
#include "SymbolTable.hpp"

typedef std::vector<Elf32_Shdr> SectionHeaderTable;
typedef std::vector<Elf32_Phdr> ProgramHeaderTable;
typedef Elf32_Ehdr Elf32Header;
typedef std::map<std::string, CustomSection> CustomSectionMap;
typedef std::map<CustomSection*, RelocationTable> RelocationTableMap;

#define ELF32FILE_NONE ET_NONE
#define ELF32FILE_REL  ET_REL
#define ELF32FILE_EXEC ET_EXEC

class Elf32File {
public:

    Elf32File();                        // Used for creating a new empty ELF file
    Elf32File(std::string _file_name);  // Used for reading an existing ELF file

    void write(std::string _file_name, Elf32_Half _type);
    static void readElf(std::string _file_name);

    Elf32Header& elf32Header() { return elf32_header; }
    SectionHeaderTable& sectionHeaderTable() { return sh_table; }
    StringTable& stringTable() { return str_table; }
    SymbolTable& symbolTable() { return sym_table; }
    CustomSectionMap& customSectionMap() { return custom_sections; }
    RelocationTableMap& relocationTableMap() { return relocation_tables; }
    ProgramHeaderTable& programHeaderTable() { return ph_table; }

    CustomSection* newCustomSection(const std::string& _name);
    CustomSection* newCustomSection(const std::string& _name, Elf32_Shdr _section_header,
                                    const std::vector<char>& _data);

    RelocationTable* newRelocationTable(CustomSection* _linked_section);
    RelocationTable* newRelocationTable(CustomSection* _linked_section, Elf32_Shdr _section_header,
                                        const std::vector<Elf32_Rela>& _data);

    ~Elf32File() = default;

private:

    Elf32Header elf32_header;
    SectionHeaderTable sh_table;
    StringTable str_table;
    SymbolTable sym_table;
    CustomSectionMap custom_sections;
    RelocationTableMap relocation_tables;
    ProgramHeaderTable ph_table;
};