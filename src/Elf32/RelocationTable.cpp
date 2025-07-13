#include "../../inc/Elf32/RelocationTable.hpp"

#include <iomanip>

#include "../../inc/Elf32/CustomSection.hpp"
#include "../../inc/Elf32/Elf32.hpp"
#include "../../inc/Elf32/Elf32File.hpp"
#include "../../inc/Elf32/Section.hpp"
#include "../../inc/Elf32/StringTable.hpp"
#include "../../inc/Elf32/SymbolTable.hpp"

const std::string RelocationTable::NAME_PREFIX = std::string(".rela");

RelocationTable::RelocationTable(Elf32File& elf32_file, CustomSection& linked_section) :
    Section(elf32_file), _linked_section(linked_section), _relocation_table() {
    const std::string& rela_name = NAME_PREFIX + linked_section.name();
    _header.sh_name = elf32_file.stringTable().add_string(rela_name);
    _header.sh_type = SHT_RELA;
    _header.sh_entsize = sizeof(Elf32_Rela);
    _header.sh_info = _linked_section.index();
    _header.sh_link = 0;
    _header.sh_addralign = 4;
    _header.sh_size = 0;
    linked_section.setRelocationTable(this);
}

RelocationTable::RelocationTable(Elf32File& elf32_file, CustomSection& linked_section,
                                 Elf32_Shdr section_header,
                                 const std::vector<Elf32_Rela>& relocation_table) :
    Section(elf32_file, section_header), _linked_section(linked_section),
    _relocation_table(relocation_table) {
    const std::string& rela_name = NAME_PREFIX + linked_section.name();
    _header.sh_name = elf32_file.stringTable().add_string(rela_name);
    linked_section.setRelocationTable(this);
}

void RelocationTable::add_entry(Elf32_Rela rela_entry) {
    _relocation_table.push_back(rela_entry);
    _header.sh_size += sizeof(Elf32_Rela);
}

void RelocationTable::add_entry(Elf32_Addr offset, Elf32_Word info, Elf32_SWord addend) {
    Elf32_Rela rela_entry = {.r_offset = offset, .r_info = info, .r_addend = addend};
    _relocation_table.push_back(rela_entry);
    _header.sh_size += sizeof(Elf32_Rela);
}

void RelocationTable::add_entry(const std::vector<Elf32_Rela>& content) {
    _relocation_table.insert(_relocation_table.end(), content.begin(), content.end());
    _header.sh_size += content.size() * sizeof(Elf32_Rela);
}

void RelocationTable::print(std::ostream& ostream) const {
    ostream << std::endl << "Relocation table " << this->name() << ":" << std::endl;
    ostream << std::left << std::setfill(' ');
    ostream << "  ";
    ostream << std::setw(4) << "NUM";
    ostream << std::setw(9) << "OFFSET";
    ostream << std::setw(9) << "TYPE";
    ostream << std::setw(25) << "SYMBOL";
    ostream << std::setw(9) << "ADDEND";
    ostream << std::endl;
    int i = 0;
    for (Elf32_Rela rela_entry : _relocation_table) {
        Elf32_Half symbol_index = ELF32_R_SYM(rela_entry.r_info);
        Elf32_Sym* symbol = _elf32_file.symbolTable().get_symbol(symbol_index);

        const std::string& symbol_name = _elf32_file.stringTable().get_string(symbol->st_name);
        std::string relocation_type;

        switch (ELF32_R_TYPE(rela_entry.r_info)) {
        case ELF32_R_TYPE_ABS32:
            relocation_type = "ABS32";
            break;
        default:
            break;
        }

        ostream << std::right << std::dec << std::setfill(' ') << std::setw(5) << i++ << " ";
        ostream << std::hex << std::setfill('0');
        ostream << std::setw(8) << rela_entry.r_offset << " ";
        ostream << std::setw(8) << std::setfill(' ') << std::left << relocation_type << " ";
        ostream << std::setw(25) << (std::to_string(symbol_index) + " (" + symbol_name + ")");
        ostream << std::dec << std::left << std::setfill(' ');
        ostream << std::setw(8) << rela_entry.r_addend;
        ostream << std::endl;
    }
}

void RelocationTable::write(std::ofstream* file) {
    if (file->tellp() % _header.sh_addralign != 0) {
        file->write("\0", _header.sh_addralign - (file->tellp() % _header.sh_addralign));
    }

    _header.sh_offset = file->tellp();
    _header.sh_size = _relocation_table.size() * sizeof(Elf32_Rela);

    for (Elf32_Rela& rela_entry : _relocation_table) {
        file->write((char*) &rela_entry, sizeof(Elf32_Rela));
    }
}
