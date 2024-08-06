#include "../inc/SymbolStringTable.hpp"

#include "../inc/SectionHeaderStringTable.hpp"


Elf32_Half SymbolStringTable::addSymbol(const std::string& _symbol) {
    int offset = section_header.sh_size;
    for (char c : _symbol) {
        content.emplace_back(c);
    }
    content.emplace_back('\0');
    section_header.sh_size += _symbol.size() + 1;
    return offset;
}

SymbolStringTable::SymbolStringTable() : Section() {
    name = ".strtab";
    SectionHeaderStringTable::getInstance().setSectionName(this);
}
