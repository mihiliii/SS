#include "../inc/Elf32File.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "../inc/CustomSection.hpp"
#include "../inc/RelocationTable.hpp"
#include "../inc/StringTable.hpp"
#include "../inc/SymbolTable.hpp"
#include "Elf32File.hpp"

Elf32File::Elf32File()
    : elf32_header(), sh_table(), str_table(this), sym_table(this), custom_sections(), relocation_tables(), ph_table() {
    elf32_header.e_shoff = sizeof(Elf32_Ehdr);
    elf32_header.e_shentsize = sizeof(Elf32_Shdr);

    str_table.add("");
    str_table.header().sh_name = str_table.add(".strtab");
    str_table.header().sh_type = SHT_STRTAB;
    str_table.header().sh_addralign = 1;

    sym_table.header().sh_name = str_table.add(".symtab");
    sym_table.header().sh_type = SHT_SYMTAB;
    sym_table.header().sh_entsize = sizeof(Elf32_Sym);
    sym_table.header().sh_addralign = 4;
}

Elf32File::Elf32File(std::string _file_name)
    : elf32_header(), sh_table(), str_table(this), sym_table(this), custom_sections(), relocation_tables(), ph_table() {
    std::ifstream file(_file_name, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file in Elf32File::Elf32File" << _file_name << std::endl;
    }

    file.read((char*) (&elf32_header), sizeof(Elf32_Ehdr));

    for (int sht_entry = 0; sht_entry < elf32_header.e_shnum; sht_entry++) {
        Elf32_Shdr section_header;
        file.seekg(elf32_header.e_shoff + sht_entry * sizeof(Elf32_Shdr));
        file.read((char*) (&section_header), sizeof(Elf32_Shdr));

        if (section_header.sh_type == SHT_STRTAB) {
            std::vector<char> string_table_data(section_header.sh_size);
            file.seekg(section_header.sh_offset);
            file.read((char*) string_table_data.data(), section_header.sh_size);

            str_table.header() = section_header;
            str_table.replace(string_table_data);
        } else if (section_header.sh_type == SHT_SYMTAB) {
            std::vector<Elf32_Sym> symbol_table_data(section_header.sh_size / sizeof(Elf32_Sym));
            file.seekg(section_header.sh_offset);
            file.read((char*) symbol_table_data.data(), section_header.sh_size);

            sym_table.header() = section_header;
            sym_table.replace(symbol_table_data);
        } else if (section_header.sh_type == SHT_CUSTOM) {
            std::vector<char> custom_section_data(section_header.sh_size);
            file.seekg(section_header.sh_offset);
            file.read((char*) custom_section_data.data(), section_header.sh_size);

            char ch;
            std::string section_name;
            file.seekg(elf32_header.e_stroff + section_header.sh_name);
            while (file.get(ch) && ch != '\0') {
                section_name.push_back(ch);
            }
            new CustomSection(this, section_name, section_header, custom_section_data);
        } else if (section_header.sh_type == SHT_RELA) {
            std::vector<Elf32_Rela> relocation_table_data(section_header.sh_size / sizeof(Elf32_Rela));
            file.seekg(section_header.sh_offset);
            file.read((char*) relocation_table_data.data(), section_header.sh_size);

            std::string section_name;
            char ch;

            file.seekg(elf32_header.e_stroff + section_header.sh_name);
            while (file.get(ch) && ch != '\0') {
                section_name.push_back(ch);
            }

            std::string parent_section_name =
                section_name.substr(sizeof(".rela") - 1, section_name.size() - (sizeof(".rela") - 1));
            CustomSection* parent_section = custom_sections.at(parent_section_name);
            new RelocationTable(this, parent_section, section_header, relocation_table_data);
        }
    }
    for (int pht_entry = 0; pht_entry < elf32_header.e_phnum; pht_entry++) {
        Elf32_Phdr program_header;
        file.seekg(elf32_header.e_phoff + pht_entry * sizeof(Elf32_Phdr));
        file.read((char*) (&program_header), sizeof(Elf32_Phdr));
        ph_table.push_back(program_header);
    }

    file.close();
}

void Elf32File::write(std::string _file_name, Elf32_Half _type) {
    std::ofstream file;
    file.open(_file_name, std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file in Elf32File::writeToFile" << _file_name << std::endl;
        return;
    }

    // Write sections right after the ELF header:
    file.seekp(sizeof(Elf32_Ehdr), std::ios::beg);
    for (Elf32_Phdr& program_header : ph_table) {
        file.write((char*) &program_header, sizeof(Elf32_Phdr));
    }

    for (auto iterator : custom_sections) {
        iterator.second->write(&file);
    }

    elf32_header.e_stroff = file.tellp();
    // Write the string table:
    str_table.write(&file);

    // Write the symbol table:
    sym_table.write(&file);

    // Set section header table offset and number of entries in the ELF header:
    elf32_header.e_type = _type;
    elf32_header.e_shoff = file.tellp();
    elf32_header.e_shnum = sh_table.size();

    // Write the section header table:
    for (Elf32_Shdr* section_header : sh_table) {
        file.write((char*) section_header, sizeof(Elf32_Shdr));
    }

    // Write the ELF header at the beginning of the file:
    file.seekp(0, std::ios::beg);
    file.write((char*) &elf32_header, sizeof(Elf32_Ehdr));

    file.close();
}

void Elf32File::readElf(std::string _file_name) {
    std::ifstream input_file(_file_name, std::ios::binary | std::ios::in);
    if (!input_file.is_open()) {
        std::cerr << "Error: Could not open file in Elf32File::readElf: " << _file_name << std::endl;
        return;
    }

    Elf32File elf_file(_file_name);

    // clang-format off

    std::cout << "Elf Header:" << std::endl;
    switch (elf_file.elf32_header.e_type) {
        case ET_NONE:
            std::cout << "  Type: No file type" << std::endl;
            break;
        case ET_REL:
            std::cout << "  Type: Relocatable file" << std::endl;
            break;
        case ET_EXEC:
            std::cout << "  Type: Executable file" << std::endl;
            break;
        case ET_DYN:
            std::cout << "  Type: Shared object file" << std::endl;
            break;
        default:
            break;
    }

    std::cout << std::hex << "  Entry point address: 0x" << elf_file.elf32_header.e_entry << std::endl
              << std::hex << "  Section header offset: 0x" << elf_file.elf32_header.e_shoff << std::endl
              << std::dec << "  Section header entry size: " << elf_file.elf32_header.e_shentsize << " (bytes)" << std::endl
              << std::dec << "  Number of section headers: " << elf_file.elf32_header.e_shnum << std::endl
              << std::hex << "  Page header offset: 0x" << elf_file.elf32_header.e_phoff << std::endl
              << std::dec << "  Page header entry size: " << elf_file.elf32_header.e_phentsize << " (bytes)" << std::endl
              << std::dec << "  Number of page headers: " << elf_file.elf32_header.e_phnum << std::endl
              << std::hex << "  String table offset: 0x" << elf_file.elf32_header.e_stroff << std::endl
              << std::endl;

    std::cout << "Section Header Table:" << std::endl << "  " 
              << std::left << std::setfill(' ')
              << std::setw(4) << "NUM"
              << std::setw(25) << "NAME"
              << std::setw(5) << "TYPE"
              << std::setw(9) << "ADDRESS"
              << std::setw(9) << "OFFSET"
              << std::setw(9) << "SIZE"
              << std::setw(5) << "LINK"
              << std::setw(5) << "INFO"
              << std::setw(6) << "ALIGN"
              << std::setw(9) << "ENTSIZE"
              << std::endl;

    uint32_t index = 0;
    for (auto& section : elf_file.sh_table) {
        std::cout << "  "
                  << std::right << std::setw(3) << std::setfill(' ') << std::dec << index << " "
                  << std::left << std::setw(24) << elf_file.str_table.get(section->sh_name) << " "
                  << std::right << std::hex << std::setfill('0')
                  << std::setw(4) << section->sh_type << " "
                  << std::setw(8) << section->sh_addr << " "
                  << std::setw(8) << section->sh_offset << " "
                  << std::setw(8) << section->sh_size << " "
                  << std::setw(4) << section->sh_link << " "
                  << std::setw(4) << section->sh_info << " "
                  << std::dec << std::setfill(' ') << std::left 
                  << std::setw(5) << section->sh_addralign << " "
                  << std::setw(8) << section->sh_entsize << std::endl;
        index += 1;
    }

    // clang-format on

    for (auto iterator : elf_file.custom_sections) {
        iterator.second->print(std::cout);
        RelocationTable* relocation_table = iterator.second->relocationTable();
        if (!relocation_table->isEmpty()) {
            relocation_table->print(std::cout);
        }
    }

    elf_file.sym_table.print(std::cout);

    size_t bufferSize = 1024;
    std::vector<char> buffer(bufferSize);

    std::cout << std::endl << "Content of " << _file_name << ":\n";

    while (!input_file.eof()) {
        input_file.read(buffer.data(), bufferSize);
        size_t s = input_file.gcount();
        for (size_t i = 0; i < s; i++) {
            if (i % 16 == 0)
                std::cout << std::right << std::hex << std::setw(8) << std::setfill('0') << i << ": ";
            else if (i % 8 == 0)
                std::cout << std::dec << " ";

            std::cout << std::hex << std::setw(2) << std::setfill('0') << (uint32_t) (unsigned char) buffer[i] << " ";

            if ((i + 1) % 16 == 0) {
                std::cout << std::dec << " |";
                for (size_t j = i - 15; j < i + 1; j++) {
                    if (buffer[j] < 32 || buffer[j] > 126) {
                        std::cout << ".";
                    } else {
                        std::cout << buffer[j];
                    }
                    if ((j + 1) % 16 == 0) {
                        std::cout << std::dec << "|\n";
                    }
                }
            }
        }

        if (s % 16 != 0) {
            for (size_t i = 0; i < 16 - s % 16; i++) {
                std::cout << "   ";
                if ((s + i) % 8 == 0) {
                    std::cout << " ";
                }
            }
            std::cout << std::dec << " |";
            for (size_t i = s - s % 16; i < s; i++) {
                if (buffer[i] < 32 || buffer[i] > 126) {
                    std::cout << ".";
                } else {
                    std::cout << buffer[i];
                }
                if (i == s - 1) {
                    std::cout << std::dec << "|\n";
                }
            }
        }
    }

    std::cout << std::dec << std::endl;
    input_file.close();
}

CustomSection* Elf32File::newCustomSection(const std::string& _name) {
    auto return_value = custom_sections.emplace(_name, CustomSection(this, _name));
    if (!return_value.second) {
        std::cerr << "Error: Could not create custom section in Elf32File::newCustomSection" << std::endl;
        return nullptr;
    }
    return &return_value.first->second;
}

CustomSection* Elf32File::newCustomSection(const std::string& _name, Elf32_Shdr _section_header, const std::vector<char>& _data) {
    auto return_value = custom_sections.emplace(_name, CustomSection(this, _name, _section_header, _data));
    if (!return_value.second) {
        std::cerr << "Error: Could not create custom section in Elf32File::newCustomSection" << std::endl;
        return nullptr;
    }
    return &return_value.first->second;
}

RelocationTable* Elf32File::newRelocationTable(CustomSection* _linked_section) {
    auto return_value = relocation_tables.emplace(_linked_section, RelocationTable(this, _linked_section));
    if (!return_value.second) {
        std::cerr << "Error: Could not create relocation table in Elf32File::newRelocationTable" << std::endl;
        return nullptr;
    }
    return &return_value.first->second;
}
