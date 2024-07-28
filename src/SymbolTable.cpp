#include "../inc/SymbolTable.hpp"

#include "../inc/Elf32.hpp"
#include "../inc/Section.hpp"
#include "../inc/SectionHeaderStringTable.hpp"
#include "SymbolTable.hpp"

SymbolTable& SymbolTable::getInstance() {
    static SymbolTable instance;
    return instance;
}

SymbolTable::SymbolTable() : Section() {
    section_header->sh_name = SectionHeaderStringTable::getInstance().setSectionName(".symtab");
    section_header->sh_type = SHT_SYMTAB;
    section_header->sh_entsize = sizeof(Elf32_Sym);
}

void SymbolTable::appendContent(Elf32_Sym* _content, size_t _size) {
    for (size_t i = 0; i < _size; i++) {
        content.emplace_back(_content[i]);
    }
    section_header->sh_size += _size * sizeof(Elf32_Sym);
}

void SymbolTable::addSymbol(
    Section* _section, const std::string& _name, Elf32_Addr _value, Elf32_Word _size
) {

}