#include "../inc/SectionHeaderStringTable.hpp"

SectionHeaderStringTable& SectionHeaderStringTable::getInstance() {
    static SectionHeaderStringTable instance;
    return instance;
}

Elf32_Word SectionHeaderStringTable::setSectionName(const std::string& _content) {
    int offset = section_header->sh_size;
    for (char c : _content) {
        content.emplace_back(c);
    }
    content.emplace_back('\0');
    section_header->sh_size += _content.size() + 1;
    return offset;
}

SectionHeaderStringTable::SectionHeaderStringTable() : Section() {
    section_header->sh_name = setSectionName(".shstrtab");
    section_header->sh_type = SHT_SHSTRTAB;
}
