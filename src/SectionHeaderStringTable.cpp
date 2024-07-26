#include "../inc/SectionHeaderStringTable.hpp"

SectionHeaderStringTable& SectionHeaderStringTable::getInstance() {
    static SectionHeaderStringTable instance;
    return instance;
}

void SectionHeaderStringTable::appendContent(char* _content, size_t _size) {
    std::string content(_content);
    setSectionName(_content);
}

Elf32_Word SectionHeaderStringTable::setSectionName(const std::string& _content) {
    int offset = section_header.sh_size;
    for (char c : _content) {
        content.push_back(c);
    }
    content.push_back('\0');
    section_header.sh_size += _content.size() + 1;
    return offset;
}

SectionHeaderStringTable::SectionHeaderStringTable() : Section(".shstrtab") {
    section_header.sh_name = setSectionName(".shstrtab");
    section_header.sh_type = SHT_SHSTRTAB;
}
