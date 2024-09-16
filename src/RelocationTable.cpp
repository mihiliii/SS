#include "../inc/RelocationTable.hpp"

#include <iomanip>

#include "../inc/CustomSection.hpp"
#include "../inc/Elf32.hpp"
#include "../inc/Elf32File.hpp"
#include "../inc/Section.hpp"
#include "../inc/StringTable.hpp"
#include "../inc/SymbolTable.hpp"

RelocationTable::RelocationTable(Elf32File* _elf32_file, CustomSection* _linked_section)
    : Section(_elf32_file), parent_section(_linked_section) {
    section_header.sh_name = _elf32_file->getStringTable().add(std::string(".rela") + _linked_section->name());
    section_header.sh_type = SHT_RELA;
    section_header.sh_entsize = sizeof(Elf32_Rela);
    section_header.sh_info = _linked_section->index();
    section_header.sh_link = 0;
    section_header.sh_addralign = 4;
    section_header.sh_size = 0;
    parent_section->setRelocationTable(this);
    _elf32_file->getRelocationTables().insert(std::pair<CustomSection*, RelocationTable*>(parent_section, this));
}

RelocationTable::RelocationTable(
    Elf32File* _elf32_file,
    CustomSection* _linked_section,
    Elf32_Shdr _section_header,
    std::vector<Elf32_Rela> _relocation_table
)
    : Section(_elf32_file, _section_header), parent_section(_linked_section), relocation_table(_relocation_table) {
    parent_section->setRelocationTable(this);
    _elf32_file->getRelocationTables().insert(std::pair<CustomSection*, RelocationTable*>(parent_section, this));
}

void RelocationTable::print(std::ofstream& _file) const {
    _file << std::endl << "Relocation table " << this->name() << ":" << std::endl;
    _file << std::left << std::setfill(' ');
    _file << "  ";
    _file << std::setw(4) << "NUM";
    _file << std::setw(9) << "OFFSET";
    _file << std::setw(9) << "TYPE";
    _file << std::setw(25) << "SYMBOL";
    _file << std::setw(9) << "ADDEND";
    _file << std::endl;
    int i = 0;
    for (Elf32_Rela relocation_table_entry : relocation_table) {
        Elf32_Half symbol_index = ELF32_R_SYM(relocation_table_entry.r_info);
        Elf32_Sym* symbol = elf32_file->getSymbolTable().get(symbol_index);

        std::string symbol_name = elf32_file->getStringTable().get(symbol->st_name);
        std::string relocation_type;

        switch (ELF32_R_TYPE(relocation_table_entry.r_info)) {
            case ELF32_R_ABS32:
                relocation_type = "ABS32";
                break;
            default:
                break;
        }

        _file << std::right << std::dec << std::setfill(' ') << std::setw(5) << i++ << " ";
        _file << std::hex << std::setfill('0');
        _file << std::setw(8) << relocation_table_entry.r_offset << " ";
        _file << std::setw(8) << std::setfill(' ') << std::left << relocation_type << " ";
        _file << std::setw(25) << (std::to_string(symbol_index) + " (" + symbol_name + ")");
        _file << std::dec << std::left << std::setfill(' ');
        _file << std::setw(8) << relocation_table_entry.r_addend;
        _file << std::endl;
    }
}

void RelocationTable::write(std::ofstream* _file) {
    if (_file->tellp() % section_header.sh_addralign != 0) {
        _file->write("\0", section_header.sh_addralign - (_file->tellp() % section_header.sh_addralign));
    }

    section_header.sh_offset = _file->tellp();
    section_header.sh_size = relocation_table.size() * sizeof(Elf32_Rela);

    for (Elf32_Rela& relocation_table_entry : relocation_table) {
        _file->write((char*) &relocation_table_entry, sizeof(Elf32_Rela));
    }
}

void RelocationTable::add(Elf32_Rela _rela_entry) {
    relocation_table.push_back(_rela_entry);
    section_header.sh_size += sizeof(Elf32_Rela);
}

void RelocationTable::add(Elf32_Addr _offset, Elf32_Word _info, Elf32_SWord _addend) {
    Elf32_Rela rela_entry = {.r_offset = _offset, .r_info = _info, .r_addend = _addend};
    relocation_table.push_back(rela_entry);
    section_header.sh_size += sizeof(Elf32_Rela);
}
