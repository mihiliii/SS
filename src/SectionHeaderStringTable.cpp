#include "../inc/SectionHeaderStringTable.hpp"

SectionHeaderStringTable& SectionHeaderStringTable::getInstance() {
    static SectionHeaderStringTable instance;
    return instance;
}

void SectionHeaderStringTable::printContent() const {
    for (char c : content) {
        std::cout << c;
    }
    std::cout << std::endl;
}

SectionHeaderStringTable::SectionHeaderStringTable() : Section() {
    name = ".shstrtab";
    section_header.sh_type = SHT_SHSTRTAB;
    setSectionName(this);
}

void SectionHeaderStringTable::setSectionName(Section* _section) {
    _section->getSectionHeader().sh_name = this->section_header.sh_size;
    const char* c = _section->getName().c_str();
    do {
        this->content.push_back(*c);
    } while (*c++ != '\0');
    this->section_header.sh_size += _section->getName().size() + 1;
}