#include "../inc/SymbolTable.hpp"

#include <iomanip>

#include "../inc/Assembler.hpp"
#include "../inc/Instructions.hpp"
#include "../inc/Section.hpp"
#include "../inc/StringTable.hpp"

SymbolTable::SymbolTable() : Section() {
    section_header.sh_name = Assembler::string_table->addString(".symtab");
    section_header.sh_type = SHT_SYMTAB;
    section_header.sh_entsize = sizeof(Elf32_Sym);
    section_header.sh_addralign = 4;
}

Elf32_Sym* SymbolTable::addSymbol(Elf32_Sym& _symbol_entry) {
    Elf32_Sym* symbol_entry = new Elf32_Sym(_symbol_entry);
    content.emplace_back(symbol_entry);
    section_header.sh_size += sizeof(Elf32_Sym);
    return symbol_entry;
}

Elf32_Sym* SymbolTable::addSymbol(
    std::string _name, Elf32_Addr _value, bool _defined, Elf32_Half _section_index, unsigned char _info
) {
    if (getSymbol(_name) != nullptr) {
        std::cerr << "Symbol " << _name << " already exists in the symbol table." << std::endl;
        return nullptr;
    }

    Elf32_Half section_index =
        ((short) _section_index == -1) ? Assembler::current_section->getSectionHeaderTableIndex() : _section_index;

    Elf32_Sym* symbol_entry = new Elf32_Sym(
        {.st_name = Assembler::string_table->addString(_name),
         .st_info = _info,
         .st_other = 0,
         .st_shndx = section_index,
         .st_value = _value,
         .st_size = 0,
         .st_defined = _defined}
    );
    content.emplace_back(symbol_entry);
    section_header.sh_size += sizeof(Elf32_Sym);
    return symbol_entry;
}

void SymbolTable::setInfo(std::string _name, Elf32_Half _info) {
    Elf32_Sym* symbol = getSymbol(_name);
    if (symbol != nullptr)
        setInfo(symbol, _info);
}

void SymbolTable::setInfo(Elf32_Sym* _symbol, Elf32_Half _info) {
    _symbol->st_info = _info;
}

Elf32_Sym* SymbolTable::getSymbol(std::string _name) {
    for (Elf32_Sym* symbol : content) {
        if (Assembler::string_table->getString(symbol->st_name) == _name)
            return symbol;
    }
    return nullptr;
}

Elf32_Sym* SymbolTable::getSymbol(uint32_t _entry_index) {
    if (_entry_index >= content.size()) {
        std::cerr << "Symbol entry index out of bounds." << std::endl;
        exit(-1);
    }
    return content[_entry_index];
}

uint32_t SymbolTable::getSymbolEntryIndex(std::string _name) {
    for (uint32_t i = 0; i < content.size(); i++) {
        if (Assembler::string_table->getString(content[i]->st_name) == _name)
            return i;
    }
    std::cerr << "Symbol " << _name << " not found." << std::endl;
    return -1;
}

uint32_t SymbolTable::getSymbolEntryIndex(Elf32_Sym* _symbol_entry) {
    for (uint32_t i = 0; i < content.size(); i++) {
        if (content[i] == _symbol_entry)
            return i;
    }
    std::cerr << "Symbol " << Assembler::string_table->getString(_symbol_entry->st_name) << " not found." << std::endl;
    return -1;
}

void SymbolTable::defineSymbol(Elf32_Sym* _symbol_entry, Elf32_Addr _value) {
    _symbol_entry->st_value = _value;
    _symbol_entry->st_defined = true;
}


void SymbolTable::print() const {
    std::cout << "Symbol Table:" << std::endl;
    std::cout << "  ";
    std::cout << std::left << std::setfill(' ');
    std::cout << std::setw(4) << "NUM";
    std::cout << std::setw(25) << "NAME";
    std::cout << std::setw(9) << "VALUE";
    std::cout << std::setw(9) << "SIZE";
    std::cout << std::setw(9) << "TYPE";
    std::cout << std::setw(6) << "BIND";
    std::cout << std::setw(6) << "OTHER";
    std::cout << std::setw(8) << "SHINDEX";
    std::cout << "DEFINED";
    std::cout << std::endl;
    uint32_t i = 0;
    for (Elf32_Sym* c : content) {
        std::string bind;
        std::string type;
        switch (ELF32_ST_BIND(c->st_info)) {
            case STB_LOCAL:
                bind = "LOC";
                break;
            case STB_GLOBAL:
                bind = "GLOB";
                break;
            case STB_WEAK:
                bind = "WEAK";
                break;
            default:
                bind = "UNK";
                break;
        }
        switch (ELF32_ST_TYPE(c->st_info)){
            case STT_NOTYPE:
                type = "NOTYPE";
                break;
            case STT_SECTION:
                type = "SECTION";
                break;
            case STT_FILE:
                type = "FILE";
                break;
            default:
                type = "UNK";
                break;
        }
        std::cout << "  ";
        std::cout << std::right << std::setfill(' ') << std::dec;
        std::cout << std::setw(3) << i << " ";
        std::cout << std::left;
        std::cout << std::setw(24) << Assembler::string_table->getString(c->st_name) << " ";
        std::cout << std::right << std::setfill('0') << std::hex;
        std::cout << std::setw(8) << c->st_value << " ";
        std::cout << std::setw(8) << c->st_size << " ";
        std::cout << std::setfill(' ') << std::dec << std::left;
        std::cout << std::setw(8) << type << " ";
        std::cout << std::setw(5) << bind << " ";
        std::cout << std::setw(5) << (int) c->st_other << " ";
        std::cout << std::setw(7) << c->st_shndx << " ";
        std::cout << (c->st_defined ? "true" : "false");
        std::cout << std::endl;
        i += 1;
    }
}

void SymbolTable::write(std::ofstream* _file) {
    section_header.sh_size = content.size() * sizeof(Elf32_Sym);

    if (_file->tellp() % section_header.sh_addralign != 0) {
        _file->write("\0", section_header.sh_addralign - (_file->tellp() % section_header.sh_addralign));
    }

    section_header.sh_offset = _file->tellp();

    for (Elf32_Sym* symbol_entry : content) {
        _file->write((char*) symbol_entry, sizeof(Elf32_Sym));
    }
}
