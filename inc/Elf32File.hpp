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
typedef Elf32_Ehdr Elf32Header;

#define ELF32FILE_EMPTY 0
#define ELF32FILE_WRITTEN 1

#define ELF32FILE_WRITE_BIN 0
#define ELF32FILE_WRITE_TXT 1

class Elf32File {
public:

    Elf32File(std::string _file_name, bool _mode);

    void write(std::string _file_name, bool _mode);
    static void writeRawContent(std::string _input_file, std::string _output_file);

    Elf32Header& getElf32Header() { return elf32_header; }
    SectionHeaderTable& getSectionHeaderTable() { return sh_table; }
    StringTable& getStringTable() { return *str_table; }
    SymbolTable& getSymbolTable() { return *sym_table; }
    std::map<std::string, CustomSection*>& getCustomSections() { return custom_sections; }
    std::map<CustomSection*, RelocationTable*>& getRelocationTables() { return relocation_tables; }

    ~Elf32File() {
        for (auto& custom_section : custom_sections) {
            delete custom_section.second;
        }
        for (auto& relocation_table : relocation_tables) {
            delete relocation_table.second;
        }
        delete str_table;
        delete sym_table;
    };

private:

    Elf32Header elf32_header;
    SectionHeaderTable sh_table;
    StringTable* str_table;
    SymbolTable* sym_table;
    std::map<std::string, CustomSection*> custom_sections;
    std::map<CustomSection*, RelocationTable*> relocation_tables;
};