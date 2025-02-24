#include "../inc/Elf32File.hpp"

#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "../inc/CustomSection.hpp"
#include "../inc/RelocationTable.hpp"
#include "../inc/StringTable.hpp"
#include "../inc/SymbolTable.hpp"
#include "../inc/misc/Exceptions.hpp"

const Elf32_Ehdr Elf32File::kElf32HeaderInit = {.e_ident = {EI_IDENTIFIER},
                                                .e_type = ET_NONE,
                                                .e_entry = 0,
                                                .e_shoff = sizeof(Elf32_Ehdr),
                                                .e_shentsize = sizeof(Elf32_Shdr),
                                                .e_shnum = 0,
                                                .e_stroff = 0};

Elf32File::Elf32File()
    : _elf32_header(kElf32HeaderInit),
      _section_header_table(std::vector<Elf32_Shdr>({Elf32_Shdr {}})),
      _string_table(*this),
      _symbol_table(*this),
      _custom_section_map(),
      _relocation_table_map()
{
    _string_table.add_string("");
    _string_table._header.sh_name = _string_table.add_string(".strtab");
    _string_table._header.sh_type = SHT_STRTAB;
    _string_table._header.sh_addralign = 1;

    _symbol_table._header.sh_name = _string_table.add_string(".symtab");
    _symbol_table._header.sh_type = SHT_SYMTAB;
    _symbol_table._header.sh_entsize = sizeof(Elf32_Sym);
    _symbol_table._header.sh_addralign = 4;
}

Elf32File::Elf32File(const std::string& file_name)
    : _elf32_header(kElf32HeaderInit),
      _section_header_table(std::vector<Elf32_Shdr>({Elf32_Shdr {}})),
      _string_table(*this),
      _symbol_table(*this),
      _custom_section_map(),
      _relocation_table_map()
{
    load(file_name);
}

Elf32Header& Elf32File::get_elf32_header()
{
    return _elf32_header;
}

SectionHeaderTable& Elf32File::get_section_header_table()
{
    return _section_header_table;
}

StringTable& Elf32File::get_string_table()
{
    return _string_table;
}

SymbolTable& Elf32File::get_symbol_table()
{
    return _symbol_table;
}

CustomSectionMap& Elf32File::get_custom_section_map()
{
    return _custom_section_map;
}

RelocationTableMap& Elf32File::get_relocation_table_map()
{
    return _relocation_table_map;
}

void Elf32File::set_type(Elf32_Half type)
{
    _elf32_header.e_type = type;
}

void Elf32File::load(const std::string& file_name)
{
    std::ifstream file(file_name, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        THROW_EXCEPTION("Can't open file: " + file_name);
    }

    file.read((char*) (&_elf32_header), sizeof(Elf32_Ehdr));

    const char elf_ident[16] = {EI_IDENTIFIER};

    if (std::memcmp(_elf32_header.e_ident, elf_ident, 16) != 0) {
        THROW_EXCEPTION("Invalid file format: " + file_name);
    }

    for (int sht_entry = 0; sht_entry < _elf32_header.e_shnum; sht_entry++) {
        Elf32_Shdr section_header;
        file.seekg(_elf32_header.e_shoff + sht_entry * sizeof(Elf32_Shdr));
        file.read((char*) (&section_header), sizeof(Elf32_Shdr));

        if (section_header.sh_type == SHT_STRTAB) {
            std::vector<char> string_table_data(section_header.sh_size);
            file.seekg(section_header.sh_offset);
            file.read((char*) string_table_data.data(), section_header.sh_size);

            _string_table.get_header() = section_header;
            _string_table.replace_data(string_table_data);
        } else if (section_header.sh_type == SHT_SYMTAB) {
            std::vector<Elf32_Sym> symbol_table_data(section_header.sh_size / sizeof(Elf32_Sym));
            file.seekg(section_header.sh_offset);
            file.read((char*) symbol_table_data.data(), section_header.sh_size);

            _symbol_table.get_header() = section_header;
            _symbol_table.replace_data(symbol_table_data);
        } else if (section_header.sh_type == SHT_CUSTOM) {
            std::vector<char> custom_section_data(section_header.sh_size);
            file.seekg(section_header.sh_offset);
            file.read((char*) custom_section_data.data(), section_header.sh_size);

            char ch;
            std::string section_name;
            file.seekg(_elf32_header.e_stroff + section_header.sh_name);
            while (file.get(ch) && ch != '\0') {
                section_name.push_back(ch);
            }

            new_custom_section(section_name, section_header, custom_section_data);
        } else if (section_header.sh_type == SHT_RELA) {
            std::vector<Elf32_Rela> relocation_table_data(section_header.sh_size /
                                                          sizeof(Elf32_Rela));
            file.seekg(section_header.sh_offset);
            file.read((char*) relocation_table_data.data(), section_header.sh_size);

            char ch;
            std::string section_name;
            file.seekg(_elf32_header.e_stroff + section_header.sh_name);
            while (file.get(ch) && ch != '\0') {
                section_name.push_back(ch);
            }

            const size_t rela_str_size = RelocationTable::kRelaNamePrefix.size();
            const std::string linked_section_name =
                section_name.substr(rela_str_size, section_name.size() - rela_str_size);

            CustomSection& linked_section = _custom_section_map.at(linked_section_name);
            new_relocation_table(linked_section, section_header, relocation_table_data);
        }
    }

    file.close();
}

void Elf32File::save(const std::string& file_name, Elf32OutputType type)
{
    std::ofstream file;
    if (type == Elf32OutputType::OBJ) {
        file.open(file_name, std::ios::out | std::ios::binary);
        if (!file.is_open()) {
            THROW_EXCEPTION("Can't open file: " + file_name);
        }

        file.seekp(sizeof(Elf32_Ehdr), std::ios::beg);

        // Write sections right after the ELF header:
        for (auto& iterator : _custom_section_map) {
            iterator.second.write(file);
        }

        _elf32_header.e_stroff = file.tellp();

        // Write the string table:
        _string_table.write(file);

        // Write the symbol table:
        _symbol_table.write(file);

        // Set section header table offset and number of entries in the ELF header:
        _elf32_header.e_shoff = file.tellp();
        _elf32_header.e_shnum = _section_header_table.size();

        // Write the section header table:
        for (Elf32_Shdr& section_header : _section_header_table) {
            file.write((char*) &section_header, sizeof(Elf32_Shdr));
        }

        // Write the ELF header at the beginning of the file:
        file.seekp(0, std::ios::beg);
        file.write((char*) &_elf32_header, sizeof(Elf32_Ehdr));

    } else if (type == Elf32OutputType::HEX) {
        file.open(file_name, std::ios::out);
        if (!file.is_open()) {
            THROW_EXCEPTION("Can't open file: " + file_name);
        }

        Elf32_Addr address_to_write = 0;

        while (true) {
            // Write section content in next format in txt file:
            // 00000000: 00 01 02 03 04 05 06 07
            // 00000008: 08 09 0A 0B 0C 0D 0E 0F
            // ...

            CustomSection* section_to_write = nullptr;
            for (auto& iterator : _custom_section_map) {
                // If address to write matches section address, set section_to_write to that section
                // Else, find the section that has the smallest address of all sections but greater
                // than address_to_write
                if (address_to_write == iterator.second._header.sh_addr) {
                    section_to_write = &iterator.second;
                    break;
                } else if (address_to_write < iterator.second._header.sh_addr) {
                    section_to_write = &iterator.second;
                }
            }

            if (section_to_write == nullptr) {
                break;
            }

            Elf32_Addr first_address = section_to_write->_header.sh_addr;
            Elf32_Addr last_address = address_to_write + section_to_write->get_size();

            for (address_to_write = first_address; address_to_write < last_address;
                 address_to_write++) {
                if (address_to_write % 8 == 0) {
                    if (address_to_write != 0) {
                        file << std::endl;
                    }
                    file << std::hex << std::setw(8) << std::setfill('0') << address_to_write
                         << ": ";
                }

                file << std::hex << std::setw(2) << std::setfill('0')
                     << (uint32_t) (unsigned char) section_to_write->get_data(address_to_write -
                                                                              first_address)
                     << " ";
            }
        }
    }

    file.close();
}

void Elf32File::ReadElf(const std::string& file_name)
{
    std::ifstream input_file(file_name, std::ios::binary | std::ios::in);
    if (!input_file.is_open()) {
        THROW_EXCEPTION("Can't open file: " + file_name);
    }

    Elf32File elf_file(file_name);

    // clang-format off

    std::cout << "File: " << file_name << std::endl << std::endl;

    std::cout << "Elf Header:" << std::endl;
    switch (elf_file._elf32_header.e_type) {
        case ET_REL:
            std::cout << "  Type: Relocatable file" << std::endl;
            break;
        case ET_EXEC:
            std::cout << "  Type: Executable file" << std::endl;
            break;
        default:
            break;
    }

    std::cout << std::hex << "  Entry point address: 0x" << elf_file._elf32_header.e_entry << std::endl
              << std::hex << "  Section header offset: 0x" << elf_file._elf32_header.e_shoff << std::endl
              << std::dec << "  Section header entry size: " << elf_file._elf32_header.e_shentsize << " (bytes)" << std::endl
              << std::dec << "  Number of section headers: " << elf_file._elf32_header.e_shnum << std::endl
              << std::hex << "  String table offset: 0x" << elf_file._elf32_header.e_stroff << std::endl
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
    for (auto& section : elf_file._section_header_table) {
        std::cout << "  "
                  << std::right << std::setw(3) << std::setfill(' ') << std::dec << index << " "
                  << std::left << std::setw(24) << elf_file._string_table.get_string(section.sh_name) << " "
                  << std::right << std::hex << std::setfill('0')
                  << std::setw(4) << section.sh_type << " "
                  << std::setw(8) << section.sh_addr << " "
                  << std::setw(8) << section.sh_offset << " "
                  << std::setw(8) << section.sh_size << " "
                  << std::setw(4) << section.sh_link << " "
                  << std::setw(4) << section.sh_info << " "
                  << std::dec << std::setfill(' ') << std::left 
                  << std::setw(5) << section.sh_addralign << " "
                  << std::setw(8) << section.sh_entsize << std::endl;
        index += 1;
    }

    // clang-format on

    for (auto& iterator : elf_file._custom_section_map) {
        iterator.second.print(std::cout);
        if (iterator.second.has_relocation_table()) {
            iterator.second.get_relocation_table().print(std::cout);
        }
    }

    elf_file._symbol_table.print(std::cout);

    size_t bufferSize = 1024;
    std::vector<char> buffer(bufferSize);

    std::cout << std::endl << "Content of " << file_name << ":\n";

    size_t i = 0;
    size_t s = 0;
    while (!input_file.eof()) {
        input_file.read(buffer.data(), bufferSize);
        s += input_file.gcount();
        for (; i < s; i++) {
            if (i % 16 == 0)
                std::cout << std::right << std::hex << std::setw(8) << std::setfill('0') << i
                          << ": ";
            else if (i % 8 == 0)
                std::cout << std::dec << " ";

            std::cout << std::hex << std::setw(2) << std::setfill('0')
                      << (uint32_t) (unsigned char) buffer[i] << " ";

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

CustomSection& Elf32File::new_custom_section(const std::string& name)
{
    auto return_value = _custom_section_map.try_emplace(name, this, name);
    if (!return_value.second) {
        THROW_EXCEPTION("Can't create new custom section: " + name);
    }
    return return_value.first->second;
}

CustomSection& Elf32File::new_custom_section(const std::string& name, Elf32_Shdr section_header,
                                           const std::vector<char>& data)
{
    auto return_value = _custom_section_map.try_emplace(name, this, name, section_header, data);
    if (!return_value.second) {
        THROW_EXCEPTION("Can't create new custom section: " + name);
    }
    return return_value.first->second;
}

RelocationTable& Elf32File::new_relocation_table(CustomSection& linked_section)
{
    auto return_value = _relocation_table_map.try_emplace(&linked_section, *this, linked_section);
    if (!return_value.second) {
        THROW_EXCEPTION("Can't create new relocation table for section: " +
                        linked_section.get_name());
    }
    return return_value.first->second;
}

RelocationTable& Elf32File::new_relocation_table(CustomSection& linked_section,
                                               Elf32_Shdr section_header,
                                               const std::vector<Elf32_Rela>& data)
{
    auto return_value = _relocation_table_map.try_emplace(&linked_section, *this, linked_section,
                                                          section_header, data);
    if (!return_value.second) {
        THROW_EXCEPTION("Can't create new relocation table for section: " +
                        linked_section.get_name());
    }
    return return_value.first->second;
}
