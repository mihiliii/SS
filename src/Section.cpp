#include "../inc/Section.hpp"

#include "../inc/SectionHeaderTable.hpp"
#include "../inc/StringTable.hpp"

Section::Section(SectionHeaderTable* _sht, Elf32_Shdr* _section_header) : sht(_sht), section_header(_section_header), sht_index(0) {
    if (_section_header == nullptr)
        sht_index = sht->add(&section_header);
}

Section::Section(SectionHeaderTable* _sht, std::string _name, Elf32_Shdr* _section_header) : sht(_sht), section_header(_section_header), sht_index(0) {
    if (_section_header == nullptr) {
        sht_index = sht->add(&section_header);
        section_header->sh_name = sht->getStringTable()->addString(_name);
    }
}

std::string Section::getName() const {
    return sht->getStringTable()->getString(section_header->sh_name);
}
