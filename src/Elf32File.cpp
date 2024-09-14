#include "../inc/Elf32File.hpp"

#include "../inc/CustomSection.hpp"
#include "../inc/RelocationTable.hpp"
#include "../inc/StringTable.hpp"
#include "../inc/SymbolTable.hpp"

// Constructor for Elf32File, used for creating new Elf32File file from scratch
Elf32File::Elf32File(std::string _file_name, bool _write)
    : file(),
      elf32_header(),
      sh_table(),
      str_table(nullptr),
      sym_table(nullptr),
      custom_sections(),
      relocation_tables() {
    if (_write) {
        file.open(_file_name, std::ios::out | std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file in Elf32File::Elf32File" << _file_name << std::endl;
        }

        // Make sure that StringTable object is created first since Sections depend on it
        str_table = new StringTable(this);
        sym_table = new SymbolTable(this);
        elf32_header.e_shoff = sizeof(Elf32_Ehdr);
        elf32_header.e_shentsize = sizeof(Elf32_Shdr);

        return;
    }
    file.open(_file_name, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file in Elf32File::Elf32File" << _file_name << std::endl;
    }

    // Sets elf32_header
    file.read((char*) (&elf32_header), sizeof(Elf32_Ehdr));

    for (int sht_entry = 0; sht_entry < elf32_header.e_shnum; sht_entry++) {
        Elf32_Shdr section_header;
        file.seekg(elf32_header.e_shoff + sht_entry * sizeof(Elf32_Shdr));
        file.read((char*) (&section_header), sizeof(Elf32_Shdr));

        if (section_header.sh_type == SHT_STRTAB) {
            std::vector<char> string_table_data(section_header.sh_size);
            file.seekg(section_header.sh_offset);
            file.read((char*) string_table_data.data(), section_header.sh_size);

            str_table = new StringTable(this, section_header, string_table_data);
        }
        else if (section_header.sh_type == SHT_SYMTAB) {
            std::vector<Elf32_Sym> symbol_table_data(section_header.sh_size / sizeof(Elf32_Sym));
            file.seekg(section_header.sh_offset);
            file.read((char*) symbol_table_data.data(), section_header.sh_size);

            sym_table = new SymbolTable(this, section_header, symbol_table_data);
        }
        else if (section_header.sh_type == SHT_CUSTOM) {
            std::vector<char> custom_section_data(section_header.sh_size);
            file.seekg(section_header.sh_offset);
            file.read((char*) custom_section_data.data(), section_header.sh_size);

            std::string section_name;
            file.seekg(elf32_header.e_stroff);
            file.read((char*) section_name.data(), section_header.sh_name);
            new CustomSection(this, section_name, section_header, custom_section_data);
        }
        else if (section_header.sh_type == SHT_RELA) {
            std::vector<Elf32_Rela> relocation_table_data(section_header.sh_size / sizeof(Elf32_Rela));
            file.seekg(section_header.sh_offset);
            file.read((char*) relocation_table_data.data(), section_header.sh_size);

            std::string section_name;
            file.seekg(elf32_header.e_stroff);
            file.read((char*) section_name.data(), section_header.sh_name);
            CustomSection* parent_section = custom_sections[section_name];
            new RelocationTable(this, parent_section, section_header, relocation_table_data);
        }
    }

}
