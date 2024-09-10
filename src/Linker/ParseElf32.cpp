#include "../inc/Linker/ParseElf32.hpp"

#include <iostream>

#include "../inc/CustomSection.hpp"
#include "../inc/StringTable.hpp"
#include "../inc/SymbolTable.hpp"

ParseElf32::ParseElf32(std::string _file_name)
    : elf32_header(nullptr),
      section_header_table(new SectionHeaderTable()),
      symbol_table(nullptr),
      string_table(nullptr) {
    std::ifstream* elf32_file = new std::ifstream();
    elf32_file->open(_file_name, std::ios::binary);
    if (!elf32_file->is_open())
        std::cerr << "Error: Could not open file.";

    // Sets elf32_header
    Elf32_Ehdr buffer;
    elf32_file->read((char*) (&buffer), sizeof(Elf32_Ehdr));
    elf32_header = new Elf32Header(buffer);

    // First pass to get string table and symbol table
    for (int sht_entry = 0; sht_entry < elf32_header->get().e_shnum; sht_entry++) {
        Elf32_Shdr section_header_buffer;
        elf32_file->seekg(elf32_header->get().e_shoff + sht_entry * sizeof(Elf32_Shdr));
        elf32_file->read((char*) (&section_header_buffer), sizeof(Elf32_Shdr));

        section_header_table->addExisting(section_header_buffer);

        if (section_header_buffer.sh_type == SHT_STRTAB) {
            std::vector<char> string_table_data(section_header_buffer.sh_size);
            elf32_file->seekg(section_header_buffer.sh_offset);
            elf32_file->read((char*) string_table_data.data(), section_header_buffer.sh_size);

            Elf32_Shdr* section_header = section_header_table->getSectionHeaderTable()[sht_entry];
            string_table = new StringTable(section_header_table, section_header, string_table_data);
        }
        else if (section_header_buffer.sh_type == SHT_SYMTAB) {
            std::vector<Elf32_Sym> symbol_table_data(section_header_buffer.sh_size / sizeof(Elf32_Sym));
            elf32_file->seekg(section_header_buffer.sh_offset);
            elf32_file->read((char*) symbol_table_data.data(), section_header_buffer.sh_size);

            Elf32_Shdr* section_header = section_header_table->getSectionHeaderTable()[sht_entry];
            symbol_table = new SymbolTable(section_header_table, section_header, symbol_table_data);
        }
    }

    // Second pass to get other sections
    for (int sht_entry = 0; sht_entry < elf32_header->get().e_shnum; sht_entry++) {
        Elf32_Shdr section_header_buffer;
        elf32_file->seekg(elf32_header->get().e_shoff + sht_entry * sizeof(Elf32_Shdr));
        elf32_file->read((char*) (&section_header_buffer), sizeof(Elf32_Shdr));

        if (section_header_buffer.sh_type != SHT_SYMTAB && section_header_buffer.sh_type != SHT_STRTAB && 
            section_header_buffer.sh_type != SHT_RELA) {
            std::vector<char> custom_section_data(section_header_buffer.sh_size);
            elf32_file->seekg(section_header_buffer.sh_offset);
            elf32_file->read((char*) custom_section_data.data(), section_header_buffer.sh_size);

            Elf32_Shdr* section_header = section_header_table->getSectionHeaderTable()[sht_entry];
            std::string section_name = string_table->get(section_header_buffer.sh_name);
            new CustomSection(section_header_table, section_name, section_header, custom_section_data);
        }
    }

    // Third pass to get relocation tables
    for (int sht_entry = 0; sht_entry < elf32_header->get().e_shnum; sht_entry++) {
        Elf32_Shdr section_header_buffer;
        elf32_file->seekg(elf32_header->get().e_shoff + sht_entry * sizeof(Elf32_Shdr));
        elf32_file->read((char*) (&section_header_buffer), sizeof(Elf32_Shdr));

        if (section_header_buffer.sh_type == SHT_RELA) {
            std::vector<Elf32_Rela> relocation_table_data(section_header_buffer.sh_size / sizeof(Elf32_Rela));
            elf32_file->seekg(section_header_buffer.sh_offset);
            elf32_file->read((char*) relocation_table_data.data(), section_header_buffer.sh_size);

            Elf32_Shdr* section_header = section_header_table->getSectionHeaderTable()[sht_entry];
            Elf32_Shdr* parent_section_header =
                section_header_table->getSectionHeaderTable()[section_header_buffer.sh_info];
            CustomSection* section =
                CustomSection::getSectionsMap()[string_table->get(parent_section_header->sh_name)];
            new RelocationTable(section_header_table, section, section_header, relocation_table_data);
        }
    }
}

Elf32Header& ParseElf32::getElf32Header() {
    return *elf32_header;
}

SectionHeaderTable& ParseElf32::getSectionHeaderTable() {
    return *section_header_table;
}

SymbolTable& ParseElf32::getSymbolTable() {
    return *symbol_table;
}

StringTable& ParseElf32::getStringTable() {
    return *string_table;
}
