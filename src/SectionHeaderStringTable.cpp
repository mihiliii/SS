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
    setSectionName(this, ".shstrtab");
    section_header.sh_type = SHT_SHSTRTAB;
}

void SectionHeaderStringTable::setSectionName(Section* _section, const std::string& _content) {
    _section->getSectionHeader().sh_name = this->section_header.sh_size;
    const char* c = _content.c_str();
    do {
        this->content.push_back(*c);
    } while (*c++ != '\0');
    this->section_header.sh_size += _content.size() + 1;
}

std::string SectionHeaderStringTable::getSectionName(Section* _section) const {
    std::string name;
    for (size_t i = _section->getSectionHeader().sh_name; this->content[i] != '\0'; i++) {
        name.push_back(this->content[i]);
    }
    return name;
};