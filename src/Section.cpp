#include "../inc/Section.hpp"

#include "../inc/Assembler.hpp"
#include "Section.hpp"

std::string Section::getName() const { return std::string(Assembler::string_table->getString(section_header.sh_name)); }

std::string Section::getName(uint32_t _index) {
    Elf32_Shdr* section_header = Assembler::section_header_table->getSectionHeader(_index);
    return Assembler::string_table->getString(section_header->sh_name);
}

Section::Section() : section_header(), section_header_table_index() {
    section_header_table_index = Assembler::section_header_table->insert(&section_header);
}

Section::Section(std::string _name) : section_header(), section_header_table_index() {
    section_header.sh_name = Assembler::string_table->addString(_name);
    section_header_table_index = Assembler::section_header_table->insert(&section_header);
}

Section::~Section() {
    // auto it = section_table.find(name);
    // if (it != section_table.end() && it->second == this) {
    // section_table.erase(it);
    //}
}
