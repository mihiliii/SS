#include "../inc/RelocationTable.hpp"
#include "../inc/Section.hpp"

#include "../inc/InputSection.hpp"
#include "../inc/Elf32.hpp"
#include "../inc/StringTable.hpp"

RelocationTable::RelocationTable(InputSection* _linked_section) : Section(std::string(".rela") + _linked_section->getName()) {
    StringTable::getInstance().addString(name, &section_header.sh_name);
    section_header.sh_type = SHT_RELA;
    section_header.sh_entsize = sizeof(Elf32_Rela);
    section_header.sh_info = _linked_section->getSectionHeaderTableIndex();
    section_header.sh_link = StringTable::getInstance().getSectionHeaderTableIndex();
}

void RelocationTable::printContent() const {

}

void RelocationTable::write(std::ofstream* _file) {

}

void RelocationTable::addRelocation(Elf32_Rela _rela_entry) {
    relocation_table.push_back(_rela_entry);
    section_header.sh_size += sizeof(Elf32_Rela);
}
