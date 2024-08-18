#include "../inc/Section.hpp"

#include "../inc/Assembler.hpp"

std::string Section::getName() const { return std::string(Assembler::string_table->getString(section_header.sh_name)); }

Section::Section() : section_header(), section_header_table_index() {
    section_header_table_index = Assembler::section_header_table->insert(&section_header);
}

Section::~Section() {
    // auto it = section_table.find(name);
    // if (it != section_table.end() && it->second == this) {
    // section_table.erase(it);
    //}
}
