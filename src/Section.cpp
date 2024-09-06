#include "../inc/Section.hpp"

#include "../inc/SectionHeaderTable.hpp"
#include "../inc/StringTable.hpp"

Section::Section() : section_header(), section_header_table_index() {
    section_header_table_index = SectionHeaderTable::getInstance().insert(&section_header);
}

std::string Section::getName() const {
    return std::string(StringTable::getInstance().getString(section_header.sh_name));
}

std::string Section::getName(uint32_t _index) {
    Elf32_Shdr* section_header = SectionHeaderTable::getInstance().getSectionHeader(_index);
    return StringTable::getInstance().getString(section_header->sh_name);
}

Section::Section(std::string _name) : section_header(), section_header_table_index() {
    section_header.sh_name = StringTable::getInstance().addString(_name);
    section_header_table_index = SectionHeaderTable::getInstance().insert(&section_header);
}
