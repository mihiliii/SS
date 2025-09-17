#include "Elf32/Elf32File.hpp"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <string>

const unsigned char Elf32File::magic_number[EI_NIDENT] = {EI_MAG0, EI_MAG1, EI_MAG2, EI_MAG3};

Elf32File::Elf32File()
    : elf32_header({
          .e_ident = {EI_MAG0, EI_MAG1, EI_MAG2, EI_MAG3},
          .e_shoff = sizeof(Elf32_Ehdr),
          .e_shentsize = sizeof(Elf32_Shdr)
}),
      section_header_table(),
      string_table(*this),
      symbol_table(*this),
      custom_section_map(),
      rela_table_map()
{
}

Elf32File::Elf32File(const std::string& file_name)
    : elf32_header(),
      section_header_table(),
      string_table(*this),
      symbol_table(*this),
      custom_section_map(),
      rela_table_map()
{
    read(file_name);
}

void Elf32File::write_bin(const std::string& file_name, Elf32_Half file_type)
{
    std::ofstream file;
    file.open(file_name, std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Elf32File: Could not open file " + file_name);
    }

    file.seekp(sizeof(Elf32_Ehdr), std::ios::beg);

    // Write custom sections right after the ELF header:
    for (auto& [name, section] : custom_section_map) {
        section.write(file);
    }

    // Write relocation tables right after custom sections:
    for (auto& [name, rela_table] : rela_table_map) {
        rela_table.write(file);
    }

    elf32_header.e_stroff = file.tellp();

    // Write the string table:
    string_table.write(file);

    // Write the symbol table:
    symbol_table.write(file);

    // Set section header table offset and number of entries in the ELF header:
    elf32_header.e_type = file_type;
    elf32_header.e_shoff = file.tellp();
    elf32_header.e_shnum = section_header_table.size();

    // Write the section header table:
    for (const Elf32_Shdr* section_header : section_header_table) {
        file.write((const char*) section_header, sizeof(Elf32_Shdr));
    }

    // Write the ELF header at the beginning of the file:
    file.seekp(0, std::ios::beg);
    file.write((const char*) &elf32_header, sizeof(Elf32_Ehdr));

    file.close();
}

void Elf32File::write_hex(const std::string& file_name)
{
    std::ofstream file;
    file.open(file_name, std::ios::out);

    if (!file.is_open()) {
        throw std::runtime_error("Elf32File: Could not open file " + file_name);
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

    for (auto& it : custom_section_map) {
        CustomSection& section = it.second;
        segments.push_back({section.get_header().sh_addr,
                            (Elf32_Addr) (section.get_header().sh_addr + section.get_size()),
                            &section});
    }

    std::sort(segments.begin(), segments.end(),
              [](const Segment& a, const Segment& b) { return a.start < b.start; });

    for (size_t i = 1; i < segments.size(); ++i) {
        if (segments[i].start < segments[i - 1].end) {
            throw std::runtime_error("Elf32File: Overlapping virtual section addresses.");
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

void Elf32File::read_elf()
{
    for (int i = 0; i < EI_NIDENT; i++) {
        if (elf32_header.e_ident[i] != magic_number[i]) {
            throw std::runtime_error("Elf32File: Not a valid ELF32 file.");
        }
    }

    std::ostream cout(std::cout.rdbuf());

    // clang-format off

    cout << "Elf Header:" << std::endl;
    switch (elf32_header.e_type) {
        case ET_NONE:
            cout << "  Type: No file type" << std::endl;
            break;
        case ET_REL:
            cout << "  Type: Relocatable file" << std::endl;
            break;
        case ET_EXEC:
            cout << "  Type: Executable file" << std::endl;
            break;
        default:
            cout << "  Type: Unknown file type" << std::endl;
            break;
    }

    cout << std::hex;
    cout << "  String table offset:       0x" << elf32_header.e_stroff << std::endl
         << "  Section header offset:     0x" << elf32_header.e_shoff  << std::endl;

    cout << std::dec;
    cout << "  Section header entry size: " << elf32_header.e_shentsize << " bytes" << std::endl
         << "  Number of section headers: " << elf32_header.e_shnum                 << std::endl
         << std::endl;

    cout << std::left;
    cout << "Section Header Table:\n  "
         << std::setw(4)  << "NUM"
         << std::setw(25) << "NAME"
         << std::setw(7)  << "TYPE"
         << std::setw(9)  << "ADDRESS"
         << std::setw(9)  << "OFFSET"
         << std::setw(9)  << "SIZE"
         << std::setw(5)  << "LINK"
         << std::setw(5)  << "INFO"
         << std::setw(6)  << "ALIGN"
         << std::setw(7)  << "ENTSIZE"
         << std::endl;

    for (size_t num = 0; num < section_header_table.size(); num++) {
        const Elf32_Shdr& section = *section_header_table[num];
        const std::string& section_name = string_table.get_string(section.sh_name);

        std::string section_type;
        switch (section.sh_type) {
        case SHT_NULL:
            section_type = "NULL";
            break;
        case SHT_SYMTAB:
            section_type = "SYMTAB";
            break;
        case SHT_STRTAB:
            section_type = "STRTAB";
            break;
        case SHT_CUSTOM:
            section_type = "CUSTOM";
            break;
        case SHT_RELA:
            section_type = "RELA";
            break;
        default:
            break;
        }

        cout << std::right << std::setfill(' ') << std::dec
             << std::setw(5)  << num                  << " "
             << std::left << std::dec << std::setfill(' ')
             << std::setw(24) << section_name         << " "
             << std::setw(6)  << section_type         << " "
             << std::right << std::hex << std::setfill('0')
             << std::setw(8)  << section.sh_addr      << " "
             << std::setw(8)  << section.sh_offset    << " "
             << std::setw(8)  << section.sh_size      << " "
             << std::setw(4)  << section.sh_link      << " "
             << std::setw(4)  << section.sh_info      << " "
             << std::dec << std::setfill(' ')
             << std::setw(5)  << section.sh_addralign << " "
             << std::setw(7)  << section.sh_entsize
             << std::endl;
    }

    // clang-format on

    for (auto& iterator : custom_section_map) {
        CustomSection& section = iterator.second;
        section.print(cout);
    }

    for (auto& iterator : rela_table_map) {
        RelocationTable& rela_table = iterator.second;
        rela_table.print(cout);
    }

    symbol_table.print(cout);

    cout << std::endl;
}

void Elf32File::read(const std::string& file_name)
{
    std::fstream file(file_name, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Elf32File: Could not open file " + file_name);
    }

    file.read((char*) (&elf32_header), sizeof(Elf32_Ehdr));

    for (int i = 0; i < EI_NIDENT; i++) {
        if (elf32_header.e_ident[i] != magic_number[i]) {
            throw std::runtime_error("Elf32File: Not a valid ELF32 file.");
        }
    }

    for (Elf32_Half sht_entry = 0; sht_entry < elf32_header.e_shnum; sht_entry++) {
        Elf32_Shdr section_header;
        file.seekg(elf32_header.e_shoff + sht_entry * sizeof(Elf32_Shdr));
        file.read((char*) (&section_header), sizeof(Elf32_Shdr));

        if (section_header.sh_type == SHT_STRTAB) {
            std::vector<char> string_table_data(section_header.sh_size);
            file.seekg(section_header.sh_offset);
            file.read((char*) string_table_data.data(), section_header.sh_size);

            string_table.set_header(section_header);
            string_table.set_string_table(string_table_data);
        }
        else if (section_header.sh_type == SHT_SYMTAB) {
            std::vector<Elf32_Sym> symbol_table_data(section_header.sh_size / sizeof(Elf32_Sym));
            file.seekg(section_header.sh_offset);
            file.read((char*) symbol_table_data.data(), section_header.sh_size);

            symbol_table.set_header(section_header);
            symbol_table.set_symbol_table(symbol_table_data);
        }
        else if (section_header.sh_type == SHT_CUSTOM) {
            std::vector<Elf32_Byte> custom_section_data(section_header.sh_size);
            file.seekg(section_header.sh_offset);
            file.read((char*) custom_section_data.data(), section_header.sh_size);

            file.seekg(elf32_header.e_stroff + section_header.sh_name);

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

            file.seekg(elf32_header.e_stroff + section_header.sh_name);

            char ch;
            std::string rela_name;
            while (file.get(ch) && ch != '\0') {
                rela_name.push_back(ch);
            }

            const std::string section_name = RelocationTable::get_custom_section_name(rela_name);

            CustomSection& custom_section = custom_section_map.at(section_name);
            new_relocation_table(rela_name, custom_section, section_header, rela_table_data);
        }
        else {
            throw std::runtime_error(
                "Elf32File: unknown section type while reading from Elf32File.");
        }
    }

    file.close();
}

CustomSection* Elf32File::new_custom_section(const std::string& name)
{
    auto pair = custom_section_map.try_emplace(name, *this, name);
    const bool& is_emplaced = pair.second;
    CustomSection* new_element = &pair.first->second;

    if (!is_emplaced) {
        throw std::runtime_error("CRITICAL: new_custom_section: could not create new section.");
    }
    return new_element;
}

CustomSection* Elf32File::new_custom_section(const std::string& name, Elf32_Shdr section_header,
                                             const std::vector<Elf32_Byte>& data)
{
    auto pair = custom_section_map.try_emplace(name, *this, name, section_header, data);
    const bool& is_emplaced = pair.second;
    CustomSection* new_element = &pair.first->second;

    if (!is_emplaced) {
        throw std::runtime_error("CRITICAL: new_custom_section: could not create new section.");
    }
    return new_element;
}

RelocationTable* Elf32File::new_relocation_table(const std::string& name,
                                                 CustomSection& linked_section)
{
    auto pair = rela_table_map.try_emplace(name, *this, linked_section);
    const bool& is_emplaced = pair.second;
    RelocationTable* new_element = &pair.first->second;

    if (!is_emplaced) {
        throw std::runtime_error(
            "CRITICAL: new_relocation_table: could not create new relocation table.");
    }
    return new_element;
}

RelocationTable* Elf32File::new_relocation_table(const std::string& name,
                                                 CustomSection& linked_section,
                                                 Elf32_Shdr section_header,
                                                 const std::vector<Elf32_Rela>& _data)
{
    auto pair = rela_table_map.try_emplace(name, *this, linked_section, section_header, _data);
    const bool& is_emplaced = pair.second;
    RelocationTable* new_element = &pair.first->second;

    if (!is_emplaced) {
        throw std::runtime_error(
            "CRITICAL: new_relocation_table: could not create new relocation table.");
    }
    return new_element;
}
