#include "../../inc/Elf32/RelocationTable.hpp"

#include "../../inc/Elf32/Elf32File.hpp"

#include <iomanip>

const std::string RelocationTable::NAME_PREFIX = std::string(".rela");

RelocationTable::RelocationTable(Elf32File& elf32_file, CustomSection& linked_section)
    : Section(elf32_file),
      _linked_section(linked_section),
      _relocation_table()
{
    const std::string& rela_name = NAME_PREFIX + linked_section.get_name();
    _header.sh_name = elf32_file.string_table.add_string(rela_name);
    _header.sh_type = SHT_RELA;
    _header.sh_entsize = sizeof(Elf32_Rela);
    _header.sh_info = _linked_section.get_index();
    _header.sh_link = 0;
    _header.sh_addralign = 4;
    _header.sh_size = 0;
    linked_section.set_rela_table(this);
}

RelocationTable::RelocationTable(Elf32File& elf32_file, CustomSection& linked_section,
                                 Elf32_Shdr section_header,
                                 const std::vector<Elf32_Rela>& relocation_table)
    : Section(elf32_file, section_header),
      _linked_section(linked_section),
      _relocation_table(relocation_table)
{
    const std::string& rela_name = NAME_PREFIX + linked_section.get_name();
    _header.sh_name = elf32_file.string_table.add_string(rela_name);
    linked_section.set_rela_table(this);
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
    return _linked_section;
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
    std::ostream os_copy(ostream.rdbuf());
    os_copy << std::endl << "Relocation table " << this->get_name() << ":" << std::endl;
    os_copy << std::left << std::setfill(' ');
    os_copy << "  ";
    os_copy << std::setw(4) << "NUM";
    os_copy << std::setw(9) << "OFFSET";
    os_copy << std::setw(9) << "TYPE";
    os_copy << std::setw(25) << "SYMBOL";
    os_copy << std::setw(9) << "ADDEND";
    os_copy << std::endl;
    int i = 0;
    for (Elf32_Rela rela_entry : _relocation_table) {
        Elf32_Half symbol_index = ELF32_R_SYM(rela_entry.r_info);
        Elf32_Sym* symbol = _elf32_file.symbol_table.get_symbol(symbol_index);

        if (symbol == nullptr) {
            std::cout << "Error";
        }

        const std::string& symbol_name = _elf32_file.string_table.get_string(symbol->st_name);
        std::string relocation_type;

        switch (ELF32_R_TYPE(rela_entry.r_info)) {
        case ELF32_R_TYPE_ABS32:
            relocation_type = "ABS32";
            break;
        default:
            break;
        }

        os_copy << std::right << std::dec << std::setfill(' ') << std::setw(5) << i++ << " ";
        os_copy << std::hex << std::setfill('0');
        os_copy << std::setw(8) << rela_entry.r_offset << " ";
        os_copy << std::setw(8) << std::setfill(' ') << std::left << relocation_type << " ";
        os_copy << std::setw(25) << (std::to_string(symbol_index) + " (" + symbol_name + ")");
        os_copy << std::dec << std::left << std::setfill(' ');
        os_copy << std::setw(8) << rela_entry.r_addend;
        os_copy << std::endl;
    }
}
