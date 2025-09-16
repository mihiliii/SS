#include "../../inc/Elf32/RelocationTable.hpp"

#include "../../inc/Elf32/Elf32File.hpp"
#include "Elf32/Elf32.hpp"

#include <algorithm>
#include <iomanip>

const std::string RelocationTable::NAME_PREFIX = std::string(".rela");

std::string RelocationTable::get_rela_name(const std::string& custom_section_name)
{
    return NAME_PREFIX + custom_section_name;
}

std::string RelocationTable::get_rela_name(CustomSection& linked_section)
{
    return NAME_PREFIX + linked_section.get_name();
}

std::string RelocationTable::get_custom_section_name(const std::string& relocation_name)
{
    size_t prefix_pos = relocation_name.find(NAME_PREFIX);
    if (prefix_pos == std::string::npos) {
        throw std::runtime_error("Error: Invalid relocation section name: " + relocation_name);
    }

    return relocation_name.substr(prefix_pos + NAME_PREFIX.length());
}

std::string RelocationTable::get_custom_section_name(RelocationTable& relocation_table)
{
    return get_custom_section_name(relocation_table.get_name());
}

RelocationTable::RelocationTable(Elf32File& elf32_file, CustomSection& linked_section)
    : Section(elf32_file),
      _linked_section(&linked_section),
      _relocation_table()
{
    const std::string& rela_name = NAME_PREFIX + linked_section.get_name();
    _header.sh_name = elf32_file.string_table.add_string(rela_name);
    _header.sh_type = SHT_RELA;
    _header.sh_entsize = sizeof(Elf32_Rela);
    _header.sh_info = _linked_section->get_header_index();
    _header.sh_link = 0;
    _header.sh_addralign = 4;
    _header.sh_size = 0;
    linked_section.set_rela_table(*this);
}

RelocationTable::RelocationTable(Elf32File& elf32_file, CustomSection& linked_section,
                                 Elf32_Shdr section_header,
                                 const std::vector<Elf32_Rela>& relocation_table)
    : Section(elf32_file, section_header),
      _linked_section(&linked_section),
      _relocation_table(relocation_table)
{
    const std::string& rela_name = NAME_PREFIX + linked_section.get_name();
    _header.sh_name = elf32_file.string_table.add_string(rela_name);
    linked_section.set_rela_table(*this);
}

RelocationTable::RelocationTable(RelocationTable&& other)
    : Section(std::move(other)),
      _linked_section(other._linked_section),
      _relocation_table(std::move(other._relocation_table))
{
    other._linked_section = nullptr;
}

RelocationTable& RelocationTable::operator=(RelocationTable&& other)
{
    if (this != &other) {
        Section::operator=(std::move(other));
        _linked_section = other._linked_section;
        _relocation_table = std::move(other._relocation_table);

        other._linked_section = nullptr;
    }
    return *this;
}

void RelocationTable::add_entry(Elf32_Rela rela_entry)
{
    _relocation_table.emplace_back(rela_entry);
    _header.sh_size += sizeof(Elf32_Rela);
}

void RelocationTable::add_entry(Elf32_Addr offset, Elf32_Word info, Elf32_SWord addend)
{
    Elf32_Rela rela_entry = {.r_offset = offset, .r_info = info, .r_addend = addend};
    _relocation_table.emplace_back(rela_entry);
    _header.sh_size += sizeof(Elf32_Rela);
}

void RelocationTable::add_entry(const std::vector<Elf32_Rela>& content)
{
    _relocation_table.insert(_relocation_table.end(), content.begin(), content.end());
    _header.sh_size += content.size() * sizeof(Elf32_Rela);
}

std::vector<Elf32_Rela>& RelocationTable::get_relocation_table()
{
    return _relocation_table;
};

CustomSection& RelocationTable::get_linked_section()
{
    return *_linked_section;
};

void RelocationTable::write(std::ostream& ostream)
{
    if (ostream.tellp() % _header.sh_addralign != 0) {
        ostream.write("\0", _header.sh_addralign - (ostream.tellp() % _header.sh_addralign));
    }

    _header.sh_offset = ostream.tellp();
    _header.sh_size = _relocation_table.size() * sizeof(Elf32_Rela);

    for (Elf32_Rela& rela_entry : _relocation_table) {
        ostream.write((char*) &rela_entry, sizeof(Elf32_Rela));
    }
}

void RelocationTable::print(std::ostream& ostream) const
{

    const Elf32_Rela max_element = *std::max_element(
        _relocation_table.begin(), _relocation_table.end(),
        [](Elf32_Rela a, Elf32_Rela b) { return ELF32_R_SYM(a.r_info) < ELF32_R_SYM(b.r_info); });

    int num_digits = std::to_string(ELF32_R_SYM(max_element.r_info)).length();

    std::ostream os_copy(ostream.rdbuf());

    os_copy << std::endl << "Relocation table " << this->get_name() << ":" << std::endl;
    os_copy << std::left << std::setfill(' ');
    os_copy << "  ";
    os_copy << std::setw(4) << "NUM";
    os_copy << std::setw(9) << "OFFSET";
    os_copy << std::setw(9) << "TYPE";
    os_copy << std::setw(25) << "SYMBOL";
    os_copy << "ADDEND";
    os_copy << std::endl;

    for (size_t i = 0; i < _relocation_table.size(); i++) {
        Elf32_Rela rela_entry = _relocation_table[i];
        Elf32_Half symbol_index = ELF32_R_SYM(rela_entry.r_info);
        Elf32_Sym* symbol = _elf32_file->symbol_table.find_symbol(symbol_index);

        if (symbol == nullptr) {
            std::cout << "Error: invalid symbol index in relocation entry." << std::endl;
        }

        const std::string& symbol_name = _elf32_file->string_table.get_string(symbol->st_name);
        std::string relocation_type;

        switch (ELF32_R_TYPE(rela_entry.r_info)) {
        case ELF32_R_TYPE_ABS32:
            relocation_type = "ABS32";
            break;
        default:
            relocation_type = "UNKNOWN";
            break;
        }

        os_copy << std::right << std::dec << std::setfill(' ') << std::setw(5) << i << " ";
        os_copy << std::hex << std::setfill('0');
        os_copy << std::setw(8) << rela_entry.r_offset << " ";
        os_copy << std::setw(8) << std::setfill(' ') << std::left << relocation_type << " ";
        os_copy << std::setw(num_digits) << std::to_string(symbol_index);
        os_copy << std::setw(25 - num_digits) << " (" + symbol_name + ")";
        os_copy << std::dec << std::right << std::setfill(' ');
        os_copy << std::setw(6) << rela_entry.r_addend;
        os_copy << std::endl;
    }
}
