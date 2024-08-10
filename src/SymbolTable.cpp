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

void SymbolTable::printContent() const {
    std::cout << "       ***  SYMBOL TABLE  ***       " << std::endl;
    std::cout << std::setw(10) << "Name" << std::setw(10) << "Value" << std::setw(4) << "Size"
              << std::setw(4) << "Info" << std::setw(4) << "Other" << std::setw(4) << "Shndx"
              << std::endl;
    for (Elf32_Sym c : content) {
        std::cout << std::setw(10) << c.st_name << std::setw(10) << c.st_value << std::setw(4)
                  << c.st_size << std::setw(4) << (int) c.st_info << std::setw(4) << (int) c.st_other
                  << std::setw(4) << c.st_shndx << std::endl;
    }
    std::cout << std::endl;
}

void SymbolTable::write(std::ofstream* _file) {
    this->section_header.sh_size = this->content.size();
    this->section_header.sh_offset = _file->tellp();

    _file->write((char*) this->content.data(), this->content.size() * sizeof(Elf32_Sym));
}

