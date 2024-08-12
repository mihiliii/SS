#include "../inc/SymbolTable.hpp"

#include <iomanip>

#include "../inc/Assembler.hpp"
#include "../inc/Section.hpp"
#include "../inc/StringTable.hpp"

SymbolTable& SymbolTable::getInstance() {
    static SymbolTable instance;
    return instance;
}

SymbolTable::SymbolTable() : Section() {
    name = ".symtab";
    StringTable::getInstance().addString(name, &section_header.sh_name);
    section_header.sh_type = SHT_SYMTAB;
    section_header.sh_entsize = sizeof(Elf32_Sym);
}

void SymbolTable::addSymbol(Elf32_Sym* _content) {
    content.emplace_back(*_content);
    section_header.sh_size += sizeof(Elf32_Sym);
}

void SymbolTable::addSymbol(std::string _name, Elf32_Addr _value) {
    Elf32_Sym symbol = {};
    StringTable::getInstance().addString(_name, &symbol.st_name);
    symbol.st_value = _value;
    symbol.st_info = 0;
    symbol.st_other = 0;
    symbol.st_shndx = Assembler::current_section->getSectionHeaderTableIndex();
    addSymbol(&symbol);
}

Elf32_Sym* SymbolTable::findSymbol(std::string& _name) {
    for (Elf32_Sym& symbol : content) {
        if (StringTable::getInstance().getString(symbol.st_name) == _name) return &symbol;
    }
    return nullptr;
}

void SymbolTable::printContent() const {
    std::cout << "       ***  SYMBOL TABLE  ***       " << std::endl;
    std::cout << "NUM " << std::setw(32) << std::left << std::setfill(' ') << "NAME" << " " << std::setw(8) << "VALUE"
              << " " << std::setw(8) << "SIZE" << " " << std::setw(8) << "INFO"
              << " " << std::setw(8) << "OTHER" << " " << std::setw(8) << "Shndx" << " " << std::endl;
    uint32_t i = 0;
    for (Elf32_Sym c : content) {
        std::cout << std::setw(3) << std::setfill(' ') << std::dec << i << " " << std::setw(32) << std::left << std::hex
                  << StringTable::getInstance().getString(c.st_name) << " " << std::setw(8) << std::setfill('0')
                  << std::hex << c.st_value << " " << std::setw(8) << c.st_size << " " << std::setw(8)
                  << (int) c.st_info << " " << std::setw(8) << (int) c.st_other << " " << std::setw(8) << std::setfill(' ') << c.st_shndx
                  << " " << std::endl;
        i += 1;
    }
    std::cout << std::endl;
}

void SymbolTable::write(std::ofstream* _file) {
    this->section_header.sh_size = this->content.size();
    this->section_header.sh_offset = _file->tellp();

    _file->write((char*) this->content.data(), this->content.size() * sizeof(Elf32_Sym));
}

