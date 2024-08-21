#include "../inc/SymbolTable.hpp"

#include <iomanip>

#include "../inc/Assembler.hpp"
#include "../inc/Section.hpp"
#include "../inc/StringTable.hpp"

SymbolTable::SymbolTable() : Section() {
    section_header.sh_name = Assembler::string_table->addString(".symtab");
    section_header.sh_type = SHT_SYMTAB;
    section_header.sh_entsize = sizeof(Elf32_Sym);
}

void SymbolTable::addSymbol(Elf32_Sym& _content) {
    content.emplace_back(_content);
    section_header.sh_size += sizeof(Elf32_Sym);
}

void SymbolTable::addSymbol(std::string _name, Elf32_Addr _value, bool _defined) {
    Elf32_Sym symbol_entry = {
        .st_name = Assembler::string_table->addString(_name),
        .st_info = 0,
        .st_other = 0,
        .st_shndx = (Elf32_Half) Assembler::current_section->getSectionHeaderTableIndex(),
        .st_value = _value,
        .st_size = 0,
        .st_defined = _defined
    };
    if (_defined == false)
        addSymbolReference(&symbol_entry, Assembler::current_section->getLocationCounter());
    addSymbol(symbol_entry);
}

void SymbolTable::setInfo(std::string _name, Elf32_Half _info) {
    Elf32_Sym* symbol = findSymbol(_name);
    if (symbol != nullptr)
        setInfo(symbol, _info);
}

void SymbolTable::setInfo(Elf32_Sym* _symbol, Elf32_Half _info) {
    _symbol->st_info = _info;
}

Elf32_Sym* SymbolTable::findSymbol(std::string _name) {
    for (Elf32_Sym& symbol : content) {
        if (Assembler::string_table->getString(symbol.st_name) == _name)
            return &symbol;
    }
    return nullptr;
}

void SymbolTable::defineSymbol(Elf32_Sym* _symbol_entry, Elf32_Addr _value) {
    _symbol_entry->st_value = _value;
    _symbol_entry->st_defined = true;
}

void SymbolTable::addSymbolReference(Elf32_Sym* _symbol_entry, Elf32_Addr _address) {
    std::string symbol_name = Assembler::string_table->getString(_symbol_entry->st_name);
    if (symbol_bp_references.find(symbol_name) == symbol_bp_references.end()) {
        symbol_bp_references[symbol_name] = std::list<Elf32_Off>();
    }
    symbol_bp_references[symbol_name].push_back(_address);
}

void SymbolTable::resolveSymbolReferences() {
    for (auto& entry : symbol_bp_references) {
        std::string symbol_name = entry.first;
        Elf32_Sym* symbol_entry = findSymbol(symbol_name);

        CustomSection* section = CustomSection::getAllSections()[Section::getName(symbol_entry->st_shndx)];
        for (Elf32_Addr& address : entry.second) {
            section->overwriteContent(&symbol_entry->st_value, sizeof(Elf32_Addr), address);
        }
    }
}

void SymbolTable::print() const {
    std::cout << "Symbol Table:" << std::endl;
    std::cout << "  ";
    std::cout << std::left << std::setfill(' ');
    std::cout << std::setw(4) << "NUM";
    std::cout << std::setw(33) << "NAME";
    std::cout << std::setw(9) << "VALUE";
    std::cout << std::setw(9) << "SIZE";
    std::cout << std::setw(5) << "INFO";
    std::cout << std::setw(6) << "OTHER";
    std::cout << std::setw(8) << "SHINDEX";
    std::cout << "DEFINED";
    std::cout << std::endl;
    uint32_t i = 0;
    for (Elf32_Sym c : content) {
        std::cout << "  ";
        std::cout << std::right << std::setfill(' ') << std::dec;
        std::cout << std::setw(3) << i << " ";
        std::cout << std::left;
        std::cout << std::setw(32) << Assembler::string_table->getString(c.st_name) << " ";
        std::cout << std::right << std::setfill('0') << std::hex;
        std::cout << std::setw(8) << c.st_value << " ";
        std::cout << std::setw(8) << c.st_size << " ";
        std::cout << std::setfill(' ') << std::dec;
        std::cout << std::setw(4) << (int) c.st_info << " ";
        std::cout << std::setw(5) << (int) c.st_other << " ";
        std::cout << std::setw(7) << c.st_shndx << " ";
        std::cout << (c.st_defined ? "true" : "false");
        std::cout << std::endl;
        i += 1;
    }
}

void SymbolTable::write(std::ofstream* _file) {
    this->section_header.sh_size = this->content.size() * sizeof(Elf32_Sym);
    this->section_header.sh_offset = _file->tellp();

    _file->write((char*) this->content.data(), this->content.size() * sizeof(Elf32_Sym));
}
