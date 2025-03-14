#include "../inc/RelocationTable.hpp"

#include <iomanip>

#include "../inc/CustomSection.hpp"
#include "../inc/Elf32.hpp"
#include "../inc/Elf32File.hpp"
#include "../inc/Section.hpp"
#include "../inc/StringTable.hpp"
#include "../inc/SymbolTable.hpp"

const std::string RelocationTable::Rela_Name_Prefix = std::string(".rela");

RelocationTable::RelocationTable(Elf32File& elf32_file, CustomSection& linked_section)
    : Section(elf32_file),
      _linked_section(linked_section),
      _string_table(elf32_file.get_string_table()),
      _symbol_table(elf32_file.get_symbol_table()),
      _relocation_table()
{
    const std::string& relocation_table_name = Rela_Name_Prefix + linked_section.get_name();
    _header.sh_name = _string_table.add_string(relocation_table_name);
    _header.sh_type = SHT_RELA;
    _header.sh_entsize = sizeof(Elf32_Rela);
    _header.sh_info = linked_section.get_index();
    _header.sh_link = 0;
    _header.sh_addralign = 4;
    _header.sh_size = 0;
    linked_section.set_relocation_table(*this);
}

RelocationTable::RelocationTable(Elf32File& elf32_file, CustomSection& linked_section,
                                 Elf32_Shdr section_header,
                                 const std::vector<Elf32_Rela>& relocation_table)
    : Section(elf32_file, section_header),
      _linked_section(linked_section),
      _string_table(elf32_file.get_string_table()),
      _symbol_table(elf32_file.get_symbol_table()),
      _relocation_table(relocation_table)
{
    const std::string& rela_table_name = Rela_Name_Prefix + linked_section.get_name();
    _header.sh_name = _string_table.add_string(rela_table_name);
    linked_section.set_relocation_table(*this);
}

const std::string& RelocationTable::get_name() const
{
    return _string_table.get_string(_header.sh_name);
}

CustomSection& RelocationTable::get_linked_section()
{
    return _linked_section;
};

void RelocationTable::replace_data(const std::vector<Elf32_Rela>& relocation_table)
{
    _relocation_table.clear();
    for (const Elf32_Rela& symbol_entry : relocation_table) {
        _relocation_table.push_back(symbol_entry);
    }
    _header.sh_size = _relocation_table.size() * sizeof(Elf32_Sym);
}

void RelocationTable::print(std::ostream& ostream) const
{
    // clang-format off
    ostream << std::endl
            << "Relocation table " << get_name() << ":" << std::endl
            << std::left << std::setfill(' ')
            << "  "
            << std::setw(4) << "NUM"
            << std::setw(9) << "OFFSET"
            << std::setw(9) << "TYPE"
            << std::setw(25) << "SYMBOL"
            << std::setw(9) << "ADDEND" << std::endl;

    int num = 0;
    for (Elf32_Rela relocation_table_entry : _relocation_table) {
        Elf32_Half symbol_index = ELF32_R_SYM(relocation_table_entry.r_info);
        const Elf32_Sym* symbol = _symbol_table.get_symbol(symbol_index);

        const std::string& symbol_name = _string_table.get_string(symbol->st_name);
        std::string relocation_type;

        switch (ELF32_R_TYPE(relocation_table_entry.r_info)) {
            case ELF32_R_TYPE_ABS32:
                relocation_type = "ABS32";
                break;
            default:
                break;
        }

        ostream << std::right << std::dec << std::setfill(' ')
                << std::setw(5) << num++
                << " " << std::hex << std::setfill('0') << std::setw(8) << relocation_table_entry.r_offset 
                << " " << std::setw(8) << std::setfill(' ') << std::left << relocation_type
                << " " << std::setw(25) << (std::to_string(symbol_index) + " (" + symbol_name + ")")
                << std::dec << std::left << std::setfill(' ') << std::setw(8) << relocation_table_entry.r_addend << std::endl;
    }
    // clang-format on
}

void RelocationTable::write(std::ofstream& file)
{
    if (file.tellp() % _header.sh_addralign != 0) {
        file.write("\0", _header.sh_addralign - (file.tellp() % _header.sh_addralign));
    }

    _header.sh_offset = file.tellp();
    _header.sh_size = _relocation_table.size() * sizeof(Elf32_Rela);

    for (Elf32_Rela& relocation_table_entry : _relocation_table) {
        file.write((char*) &relocation_table_entry, sizeof(Elf32_Rela));
    }
}

Elf32_Rela* RelocationTable::get_entry(size_t index)
{
    if (index >= _relocation_table.size()) {
        return nullptr;
    }
    return &_relocation_table[index];
}

Elf32_Rela* RelocationTable::get_entry(const std::string& symbol_name)
{
    for (Elf32_Rela& relocation_table_entry : _relocation_table) {
        Elf32_Half symbol_index = ELF32_R_SYM(relocation_table_entry.r_info);
        const Elf32_Sym* symbol = _symbol_table.get_symbol(symbol_index);
        const std::string& current_symbol_name = _string_table.get_string(symbol->st_name);

        if (current_symbol_name == symbol_name) {
            return &relocation_table_entry;
        }
    }
    return nullptr;
}

void RelocationTable::add_entry(Elf32_Rela rela_entry)
{
    _relocation_table.push_back(rela_entry);
    _header.sh_size += sizeof(Elf32_Rela);
}

void RelocationTable::add_entry(Elf32_Addr offset, Elf32_Word info, Elf32_SWord addend)
{
    Elf32_Rela rela_entry = {.r_offset = offset, .r_info = info, .r_addend = addend};
    _relocation_table.push_back(rela_entry);
    _header.sh_size += sizeof(Elf32_Rela);
}