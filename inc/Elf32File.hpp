#pragma once

#include <fstream>
#include <map>
#include <vector>

#include "CustomSection.hpp"
#include "Elf32.hpp"
#include "RelocationTable.hpp"
#include "StringTable.hpp"
#include "SymbolTable.hpp"

typedef std::vector<Elf32_Shdr*> SectionHeaderTable;
typedef std::vector<Elf32_Phdr> ProgramHeaderTable;
typedef Elf32_Ehdr Elf32Header;
typedef std::map<std::string, CustomSection*> CustomSectionMap;
typedef std::map<CustomSection*, RelocationTable*> RelocationTableMap;

#define ELF_NONE ET_NONE
#define ELF_REL  ET_REL
#define ELF_EXEC ET_EXEC

class Elf32File {
public:

    Elf32File();                        // Used for creating a new empty ELF file
    Elf32File(std::string _file_name);  // Used for reading an existing ELF file

    void write(std::string _file_name, Elf32_Half _type);
    static void readElf(std::string _file_name);

    Elf32Header& getElf32Header() { return elf32_header; }
    SectionHeaderTable& getSectionHeaderTable() { return sh_table; }
    StringTable& getStringTable() { return str_table; }
    SymbolTable& getSymbolTable() { return sym_table; }
    CustomSectionMap& getCustomSections() { return custom_sections; }
    RelocationTableMap& getRelocationTables() { return relocation_tables; }

    ~Elf32File() {
        for (auto& custom_section : custom_sections) {
            delete custom_section.second;
        }
        for (auto& relocation_table : relocation_tables) {
            delete relocation_table.second;
        }
    };

private:

    Elf32Header elf32_header;
    SectionHeaderTable sh_table;
    StringTable str_table;
    SymbolTable sym_table;
    CustomSectionMap custom_sections;
    RelocationTableMap relocation_tables;
    ProgramHeaderTable ph_table;
};