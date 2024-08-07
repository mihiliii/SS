#include "../inc/SymbolTable.hpp"

#include "../inc/Section.hpp"
#include "../inc/StringTable.hpp"

SymbolTable& SymbolTable::getInstance() {
    static SymbolTable instance;
    return instance;
}

SymbolTable::SymbolTable() : Section() {
    name = ".symtab";
    section_header.sh_name = StringTable::getInstance().addString(name);
    section_header.sh_type = SHT_SYMTAB;
    section_header.sh_entsize = sizeof(Elf32_Sym);
}

void SymbolTable::appendContent(Elf32_Sym* _content, size_t _size) {
    for (size_t i = 0; i < _size; i++) {
        content.emplace_back(_content[i]);
    }
    section_header.sh_size += _size * sizeof(Elf32_Sym);
}

void SymbolTable::printContent() const {
    for (Elf32_Sym c : content) {
        std::cout << c.st_name << " " << c.st_value << " " << c.st_size << " " << c.st_info << " " << c.st_other << " "
                  << c.st_shndx << std::endl;
    }
    std::cout << std::endl;
}
