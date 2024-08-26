#include "../inc/RelocationTable.hpp"

#include "../inc/Assembler.hpp"
#include "../inc/CustomSection.hpp"
#include "../inc/Elf32.hpp"
#include "../inc/Section.hpp"
#include "../inc/StringTable.hpp"

RelocationTable::RelocationTable(CustomSection* _linked_section) : Section(), parent_section(_linked_section) {
    section_header.sh_name = Assembler::string_table->addString(std::string(".rela") + _linked_section->getName());
    section_header.sh_type = SHT_RELA;
    section_header.sh_entsize = sizeof(Elf32_Rela);
    section_header.sh_info = _linked_section->getSectionHeaderTableIndex();
    section_header.sh_link = Assembler::symbol_table->getSectionHeaderTableIndex();
    section_header.sh_addralign = 4;
    section_header.sh_size = 0;
}

void RelocationTable::print() const {}

void RelocationTable::write(std::ofstream* _file) {

    if (_file->tellp() % section_header.sh_addralign != 0) {
        _file->write("\0", section_header.sh_addralign - (_file->tellp() % section_header.sh_addralign));
    }

    section_header.sh_offset = _file->tellp();

    for (Elf32_Rela& relocation_table_entry : relocation_table) {
        _file->write((char*) &relocation_table_entry, sizeof(Elf32_Rela));
    }
}

void RelocationTable::add(Elf32_Rela _rela_entry) {
    relocation_table.push_back(_rela_entry);
    section_header.sh_size += sizeof(Elf32_Rela);
}

void RelocationTable::add(Elf32_Addr _offset, Elf32_Word _info, Elf32_SWord _addend) {
    Elf32_Rela rela_entry = {
        .r_offset = _offset,
        .r_info = _info,
        .r_addend = _addend
    };
    relocation_table.push_back(rela_entry);
    section_header.sh_size += sizeof(Elf32_Rela);
}
