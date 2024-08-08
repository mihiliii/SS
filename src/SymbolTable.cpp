#include "../inc/SymbolTable.hpp"
#include "../inc/Assembler.hpp"

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

void SymbolTable::appendContent(Elf32_Sym* _content) {
    content.emplace_back(*_content);
    section_header.sh_size += sizeof(Elf32_Sym);
}

void SymbolTable::appendContent(std::string _name, Elf32_Addr _value) {
    Elf32_Sym symbol = {};
    symbol.st_name = StringTable::getInstance().addString(_name);
    symbol.st_value = _value;
    symbol.st_info = 0;
    symbol.st_other = 0;
    symbol.st_shndx = Assembler::current_section->getSectionHeaderTableIndex();
    appendContent(&symbol);
}

void SymbolTable::printContent() const {
    for (Elf32_Sym c : content) {
        std::cout << c.st_name << " " << c.st_value << " " << c.st_size << " " << c.st_info << " " << c.st_other << " "
                  << c.st_shndx << std::endl;
    }
    std::cout << std::endl;
}
