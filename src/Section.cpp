#include "../inc/Section.hpp"

#include "../inc/SectionHeaderTable.hpp"
#include "../inc/StringTable.hpp"

Section::Section(SectionHeaderTable* _sht) : sht(_sht), section_header(nullptr), sht_index(0) {
    sht_index = sht->add(&section_header);
}

Section::Section(SectionHeaderTable* _sht, std::string _name) : sht(_sht), section_header(nullptr), sht_index(0) {
    sht_index = sht->add(&section_header);
    section_header->sh_name = sht->getStringTable()->addString(_name);
}

std::string Section::getName() const {
    return std::string(sht->getStringTable()->getString(section_header->sh_name));
}
