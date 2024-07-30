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
