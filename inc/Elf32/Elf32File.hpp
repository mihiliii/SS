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
typedef std::map<CustomSection*, RelocationTable> RelocationTableMap;

#define ELF32FILE_NONE ET_NONE
#define ELF32FILE_REL  ET_REL
#define ELF32FILE_EXEC ET_EXEC

class Elf32File {
public:

    Elf32File();                             // Used for creating a new empty ELF file
    Elf32File(const std::string& file_name); // Used for reading an existing ELF file

    void write(std::string file_name, Elf32_Half type);
    void writeHex(std::string file_name);
    static void readElf(std::string file_name);

    Elf32Header& elf32Header() { return _elf32_header; }
    SectionHeaderTable& sectionHeaderTable() { return _sh_table; }
    StringTable& stringTable() { return _str_table; }
    SymbolTable& symbolTable() { return _sym_table; }
    CustomSectionMap& customSectionMap() { return _custom_sections; }
    RelocationTableMap& relocationTableMap() { return _relocation_tables; }

    CustomSection* newCustomSection(const std::string& name);
    CustomSection* newCustomSection(const std::string& name, Elf32_Shdr section_header,
                                    const std::vector<char>& data);

    RelocationTable* newRelocationTable(CustomSection* linked_section);
    RelocationTable* newRelocationTable(CustomSection* linked_section, Elf32_Shdr section_header,
                                        const std::vector<Elf32_Rela>& data);

    ~Elf32File() = default;

private:

    Elf32Header _elf32_header;
    SectionHeaderTable _sh_table;
    StringTable _str_table;
    SymbolTable _sym_table;
    CustomSectionMap _custom_sections;
    RelocationTableMap _relocation_tables;
};
