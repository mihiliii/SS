#include "../inc/RelocationTable.hpp"
#include "../inc/Section.hpp"
#include "../inc/Assembler.hpp"

#include "../inc/CustomSection.hpp"
#include "../inc/Elf32.hpp"
#include "../inc/StringTable.hpp"

RelocationTable::RelocationTable(CustomSection* _linked_section) : Section() {
    section_header.sh_name = Assembler::string_table->addString(std::string(".rela") + _linked_section->getName());
    section_header.sh_type = SHT_RELA;
    section_header.sh_entsize = sizeof(Elf32_Rela);
    section_header.sh_info = _linked_section->getSectionHeaderTableIndex();
    section_header.sh_link = Assembler::string_table->getSectionHeaderTableIndex();
}

void RelocationTable::print() const {

}

void RelocationTable::write(std::ofstream* _file) {

}

void RelocationTable::addRelocation(Elf32_Rela _rela_entry) {
    relocation_table.push_back(_rela_entry);
    section_header.sh_size += sizeof(Elf32_Rela);
}
