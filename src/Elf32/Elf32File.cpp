#include "../../inc/Elf32/Elf32File.hpp"

#include "Elf32/CustomSection.hpp"
#include "Elf32/Elf32.hpp"
#include "Elf32/StringTable.hpp"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <string>

Elf32File::Elf32File()
    : _elf32_header({.e_shoff = sizeof(Elf32_Ehdr), .e_shentsize = sizeof(Elf32_Shdr)}),
      _section_header_table(),
      _string_table(*this),
      _symbol_table(*this),
      _custom_section_map(),
      _rela_table_map()
{
}

Elf32File::Elf32File(const std::string& file_name)
    : _elf32_header(),
      _section_header_table(),
      _string_table(*this),
      _symbol_table(*this),
      _custom_section_map(),
      _rela_table_map()
{
    read(file_name);
}

void Elf32File::write_bin(const std::string& file_name, Elf32_Half file_type)
{
    std::ofstream file;
    file.open(file_name, std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file in Elf32File::write " << file_name << std::endl;
        return;
    }

    file.seekp(sizeof(Elf32_Ehdr), std::ios::beg);

    // TODO: change to Section* map when writing to file

    // Write custom sections right after the ELF header:
    for (auto& iterator : _custom_section_map) {
        CustomSection& section = iterator.second;
        section.write(file);
    }

    // Write relocation tables right after custom sections:
    for (auto& iterator : _rela_table_map) {
        RelocationTable& rela_table = iterator.second;
        rela_table.write(file);
    }

    // TODO: check if stroff is needed

    _elf32_header.e_stroff = file.tellp();

    // Write the string table:
    _string_table.write(file);

    // Write the symbol table:
    _symbol_table.write(file);

    // Set section header table offset and number of entries in the ELF header:
    _elf32_header.e_type = file_type;
    _elf32_header.e_shoff = file.tellp();
    _elf32_header.e_shnum = _section_header_table.size();

    // Write the section header table:
    for (const Elf32_Shdr& section_header : _section_header_table) {
        file.write((const char*) &section_header, sizeof(Elf32_Shdr));
    }

    // Write the ELF header at the beginning of the file:
    file.seekp(0, std::ios::beg);
    file.write((const char*) &_elf32_header, sizeof(Elf32_Ehdr));

    file.close();
}

void Elf32File::write_hex(const std::string& file_name)
{
    std::ofstream file;
    file.open(file_name, std::ios::out);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file in Elf32File::writeToFile" << file_name
                  << std::endl;
        return;
    }

    struct Segment {
        Elf32_Addr start;
        Elf32_Addr end;
        CustomSection* section;
    };

    std::vector<Segment> segments;

    // Write section content in next format in txt file:
    // 00000000: 00 01 02 03 04 05 06 07
    // 00000008: 08 09 0A 0B 0C 0D 0E 0F

    for (auto& it : _custom_section_map) {
        CustomSection& section = it.second;
        segments.push_back({section.get_header().sh_addr,
                            (Elf32_Addr) (section.get_header().sh_addr + section.get_size()),
                            &section});
    }

    std::sort(segments.begin(), segments.end(),
              [](const Segment& a, const Segment& b) { return a.start < b.start; });

    for (size_t i = 1; i < segments.size(); ++i) {
        if (segments[i].start < segments[i - 1].end) {
            std::cout << "Error: Overlapping sections in Elf32File::writeHex" << std::endl;
        }
    }

    file << std::hex << std::setfill('0') << std::uppercase;

    for (auto segment : segments) {
        const std::vector<Elf32_Byte>& data = segment.section->get_data();
        for (Elf32_Addr file_address = segment.start; file_address < segment.end; file_address++) {
            if (file_address % 8 == 0) {
                file << std::setw(8) << file_address << ": ";
            }
            file << std::setw(2) << (uint32_t) data[file_address - segment.start] << " ";
        }
    }

    file.close();
}

void Elf32File::read_elf(const std::string& file_name)
{
    std::fstream input_file(file_name, std::ios::binary | std::ios::in);
    if (!input_file.is_open()) {
        std::cerr << "Error: Could not open file in Elf32File::readElf: " << file_name << std::endl;
        return;
    }

    std::ostream cout(std::cout.rdbuf());

    // clang-format off

    cout << "ReadElf: " << file_name << std::endl << std::endl;

    cout << "Elf Header: " << std::endl;
    switch (_elf32_header.e_type) {
        case ET_NONE:
            cout << "  Type: No file type" << std::endl;
            break;
        case ET_REL:
            cout << "  Type: Relocatable file" << std::endl;
            break;
        case ET_EXEC:
            cout << "  Type: Executable file" << std::endl;
            break;
        case ET_DYN:
            cout << "  Type: Shared object file" << std::endl;
            break;
        default:
            cout << "  Type: Unknown file type" << std::endl;
            break;
    }

    // TODO: remove unused header entries 

    cout << std::hex;
    cout << "  Entry point address:   0x" << _elf32_header.e_entry  << std::endl
         << "  String table offset:   0x" << _elf32_header.e_stroff << std::endl
         << "  Section header offset: 0x" << _elf32_header.e_shoff  << std::endl;

    cout << std::dec;
    cout << "  Section header entry size: " << _elf32_header.e_shentsize << " (bytes)" << std::endl
         << "  Number of section headers: " << _elf32_header.e_shnum                   << std::endl
         << "  Program header entry size: " << _elf32_header.e_phentsize << " (bytes)" << std::endl
         << "  Number of program headers: " << _elf32_header.e_phnum                   << std::endl
         << std::endl;

    cout << std::left << std::setfill(' ') << "Section Header Table: \n  "
         << std::setw(4)  << "NUM"
         << std::setw(25) << "NAME"
         << std::setw(5)  << "TYPE"
         << std::setw(9)  << "ADDRESS"
         << std::setw(9)  << "OFFSET"
         << std::setw(9)  << "SIZE"
         << std::setw(5)  << "LINK"
         << std::setw(5)  << "INFO"
         << std::setw(6)  << "ALIGN"
         << std::setw(9)  << "ENTSIZE"
         << std::endl;

    for (size_t num = 0; num < _section_header_table.size(); num++) {
        const auto& section = _section_header_table[num];
        const std::string& section_name = _string_table.get_string(section.sh_name);
        cout << std::right << std::setfill(' ') << std::dec 
             << std::setw(5)  << num                  << " "
             << std::left << std::dec << std::setfill(' ')
             << std::setw(24) << section_name         << " "
             << std::right << std::hex << std::setfill('0')
             << std::setw(4)  << section.sh_type      << " "
             << std::setw(8)  << section.sh_addr      << " "
             << std::setw(8)  << section.sh_offset    << " "
             << std::setw(4)  << section.sh_link      << " "
             << std::setw(8)  << section.sh_size      << " "
             << std::setw(4)  << section.sh_info      << " "
             << std::left << std::dec << std::setfill(' ')
             << std::setw(5)  << section.sh_addralign << " "
             << std::setw(8)  << section.sh_entsize
             << std::endl;
    }

    // clang-format on

    for (auto& iterator : _custom_section_map) {
        CustomSection& section = iterator.second;
        cout << "Section content " << section.get_name() << ":\n";
        section.print(cout);
    }
    for (auto& iterator : _rela_table_map) {
        RelocationTable& rela_table = iterator.second;
        rela_table.print(cout);
    }

    _symbol_table.print(cout);

    const size_t buffer_size = 1024;
    std::vector<char> buffer(buffer_size);

    cout << std::endl << "Content of " << file_name << ":" << std::endl;

    // TODO: change how it works
    size_t i = 0;
    size_t s = 0;
    while (!input_file.eof()) {
        input_file.read(buffer.data(), buffer_size);
        s += input_file.gcount();
        for (; i < s; i++) {
            if (i % 16 == 0) {
                cout << std::right << std::hex << std::setw(8) << std::setfill('0') << i << ": ";
            }
            else if (i % 8 == 0) {
                cout << std::dec << " ";
            }

            cout << std::hex << std::setw(2) << std::setfill('0') << (uint32_t) buffer[i] << " ";

            if ((i + 1) % 16 == 0) {
                cout << std::dec << " |";
                for (size_t j = i - 15; j < i + 1; j++) {
                    if (buffer[j] < 32 || buffer[j] > 126) {
                        cout << ".";
                    }
                    else {
                        cout << buffer[j];
                    }
                    if ((j + 1) % 16 == 0) {
                        cout << std::dec << "|\n";
                    }
                }
            }
        }

        if (s % 16 != 0) {
            for (size_t i = 0; i < 16 - s % 16; i++) {
                cout << "   ";
                if ((s + i) % 8 == 0) {
                    cout << " ";
                }
            }
            cout << std::dec << " |";
            for (size_t i = s - s % 16; i < s; i++) {
                if (buffer[i] < 32 || buffer[i] > 126) {
                    cout << ".";
                }
                else {
                    cout << buffer[i];
                }
                if (i == s - 1) {
                    cout << std::dec << "|\n";
                }
            }
        }
    }
    cout << std::endl;
    input_file.close();
}

void Elf32File::read(const std::string& file_name)
{
    std::fstream file(file_name, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file in Elf32File::read: " << file_name << std::endl;
        return;
    }

    file.read((char*) (&_elf32_header), sizeof(Elf32_Ehdr));

    for (Elf32_Half sht_entry = 0; sht_entry < _elf32_header.e_shnum; sht_entry++) {
        Elf32_Shdr section_header;
        file.seekg(_elf32_header.e_shoff + sht_entry * sizeof(Elf32_Shdr));
        file.read((char*) (&section_header), sizeof(Elf32_Shdr));

        if (section_header.sh_type == SHT_STRTAB) {
            std::vector<char> string_table_data(section_header.sh_size);
            file.seekg(section_header.sh_offset);
            file.read((char*) string_table_data.data(), section_header.sh_size);

            _string_table.set_header(section_header);
            _string_table.set_string_table(string_table_data);
        }
        else if (section_header.sh_type == SHT_SYMTAB) {
            std::vector<Elf32_Sym> symbol_table_data(section_header.sh_size / sizeof(Elf32_Sym));
            file.seekg(section_header.sh_offset);
            file.read((char*) symbol_table_data.data(), section_header.sh_size);

            _symbol_table.set_header(section_header);
            _symbol_table.set_symbol_table(symbol_table_data);
        }
        else if (section_header.sh_type == SHT_CUSTOM) {
            std::vector<char> custom_section_data(section_header.sh_size);
            file.seekg(section_header.sh_offset);
            file.read((char*) custom_section_data.data(), section_header.sh_size);

            file.seekg(_elf32_header.e_stroff + section_header.sh_name);

            char ch;
            std::string section_name;
            while (file.get(ch) && ch != '\0') {
                section_name.push_back(ch);
            }

            new_custom_section(section_name, section_header, custom_section_data);
        }
        else if (section_header.sh_type == SHT_RELA) {
            std::vector<Elf32_Rela> rela_table_data(section_header.sh_size / sizeof(Elf32_Rela));
            file.seekg(section_header.sh_offset);
            file.read((char*) rela_table_data.data(), section_header.sh_size);

            file.seekg(_elf32_header.e_stroff + section_header.sh_name);

            char ch;
            std::string rela_section_name;
            while (file.get(ch) && ch != '\0') {
                rela_section_name.push_back(ch);
            }

            // TODO: introduce new std::vector<Section*>
            const size_t rela_str_size = RelocationTable::NAME_PREFIX.size();
            std::string custom_section_name =
                rela_section_name.substr(rela_str_size, rela_section_name.size() - rela_str_size);

            CustomSection& custom_section = _custom_section_map.at(custom_section_name);
            new_relocation_table(rela_section_name, custom_section, section_header,
                                 rela_table_data);
        }
        else {
            std::cout << "Error: Unknown section type in Elf32File::read: "
                      << section_header.sh_type << std::endl;
        }
    }

    file.close();
}

CustomSection* Elf32File::new_custom_section(const std::string& name)
{
    auto pair = _custom_section_map.try_emplace(name, this, name);
    const bool& is_emplaced = pair.second;
    CustomSection* new_element = &pair.first->second;

    if (!is_emplaced) {
        std::cerr << "Error: Could not create custom section in Elf32File::newCustomSection"
                  << std::endl;
        return nullptr;
    }
    else {
        return new_element;
    }
}

CustomSection* Elf32File::new_custom_section(const std::string& name, Elf32_Shdr section_header,
                                             const std::vector<char>& _data)
{
    auto pair = _custom_section_map.try_emplace(name, this, name, section_header, _data);
    const bool& is_emplaced = pair.second;
    CustomSection* new_element = &pair.first->second;

    if (!is_emplaced) {
        std::cerr << "Error: Could not create custom section in Elf32File::newCustomSection"
                  << std::endl;
        return nullptr;
    }
    else {
        return new_element;
    }
}

RelocationTable* Elf32File::new_relocation_table(const std::string& name,
                                                 CustomSection& linked_section)
{
    auto pair = _rela_table_map.try_emplace(name, this, linked_section);
    const bool& is_emplaced = pair.second;
    RelocationTable* new_element = &pair.first->second;

    if (!is_emplaced) {
        std::cerr << "Error: Could not create relocation table in Elf32File::newRelocationTable"
                  << std::endl;
        return nullptr;
    }
    else {
        return new_element;
    }
}

RelocationTable* Elf32File::new_relocation_table(const std::string& name,
                                                 CustomSection& linked_section,
                                                 Elf32_Shdr section_header,
                                                 const std::vector<Elf32_Rela>& _data)
{
    auto pair = _rela_table_map.try_emplace(name, this, linked_section, section_header, _data);
    const bool& is_emplaced = pair.second;
    RelocationTable* new_element = &pair.first->second;

    if (!is_emplaced) {
        std::cerr << "Error: Could not create relocation table in Elf32File::newRelocationTable"
                  << std::endl;
        return nullptr;
    }
    else {
        return new_element;
    }
}
