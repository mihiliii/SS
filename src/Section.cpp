#include "../inc/Section.hpp"

std::map<std::string, Section*> Section::section_table;

Section::Section() : section_header(), name(), section_header_table_index(0) {
    section_header_table_index = SectionHeaderTable::getInstance().insert(&section_header);
    section_table.insert({name, this}); // insertion check needs to be added
}

Section::~Section() {
    auto it = section_table.find(name);
    if (it != section_table.end() && it->second == this) {
        section_table.erase(it);
    }
}


std::string Section::getName() const { return name; }

uint32_t Section::getSectionHeaderTableIndex() const { return section_header_table_index; }

Elf32_Shdr& Section::getSectionHeader() { return section_header; }
