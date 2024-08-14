#include "../inc/SymbolTable.hpp"

#include <iomanip>

#include "../inc/Assembler.hpp"
#include "../inc/Section.hpp"
#include "../inc/StringTable.hpp"

SymbolTable& SymbolTable::getInstance() {
    static SymbolTable instance;
    return instance;
}

SymbolTable::SymbolTable() : Section(std::string(".symtab")) {
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
    std::cout << "Symbol Table:" << std::endl;
    std::cout << "  ";
    std::cout << std::left << std::setfill(' ');
    std::cout << std::setw(4) << "NUM";
    std::cout << std::setw(33) << "NAME";
    std::cout << std::setw(9) << "VALUE";
    std::cout << std::setw(9) << "SIZE";
    std::cout << std::setw(5) << "INFO";
    std::cout << std::setw(6) << "OTHER";
    std::cout << std::setw(9) << "SHINDEX";
    std::cout << std::endl;
    uint32_t i = 0;
    for (Elf32_Sym c : content) {
        std::cout << "  ";
        std::cout << std::right << std::setfill(' ') << std::dec;
        std::cout << std::setw(3) << i << " ";
        std::cout << std::left;
        std::cout << std::setw(32) << StringTable::getInstance().getString(c.st_name) << " ";
        std::cout << std::right << std::setfill('0') << std::hex;
        std::cout << std::setw(8) << c.st_value << " " ;
        std::cout << std::setw(8) << c.st_size << " ";
        std::cout << std::setfill(' ') << std::dec;
        std::cout << std::setw(4) << (int) c.st_info << " ";
        std::cout << std::setw(5) << (int) c.st_other << " ";
        std::cout << std::setw(7) << c.st_shndx << std::endl;
        i += 1;
    }
}

void SymbolTable::write(std::ofstream* _file) {
    this->section_header.sh_size = this->content.size() * sizeof(Elf32_Sym);
    this->section_header.sh_offset = _file->tellp();


    _file->write((char*) this->content.data(), this->content.size() * sizeof(Elf32_Sym));
}

