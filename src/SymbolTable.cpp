#include "../inc/SymbolTable.hpp"

#include <iomanip>

#include "../inc/Assembler/Assembler.hpp"
#include "../inc/Assembler/Instructions.hpp"
#include "../inc/Section.hpp"
#include "../inc/StringTable.hpp"
#include "../inc/SectionHeaderTable.hpp"

SymbolTable::SymbolTable(SectionHeaderTable* _sht) : Section(_sht) {
    section_header->sh_name = _sht->getStringTable()->addString(".symtab");
    section_header->sh_type = SHT_SYMTAB;
    section_header->sh_entsize = sizeof(Elf32_Sym);
    section_header->sh_addralign = 4;
    _sht->setSymbolTable(this);
}

Elf32_Sym* SymbolTable::addSymbol(Elf32_Sym& _symbol_entry) {
    Elf32_Sym* symbol_entry = new Elf32_Sym(_symbol_entry);
    content.emplace_back(symbol_entry);
    section_header->sh_size += sizeof(Elf32_Sym);
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
        {.st_name = sht->getStringTable()->addString(_name),
         .st_info = _info,
         .st_shndx = section_index,
         .st_value = _value,
         .st_size = 0,
         .st_defined = _defined}
    );
    content.emplace_back(symbol_entry);
    section_header->sh_size += sizeof(Elf32_Sym);
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
        if (sht->getStringTable()->getString(symbol->st_name) == _name)
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
        if (sht->getStringTable()->getString(content[i]->st_name) == _name)
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
    std::cerr << "Symbol " << sht->getStringTable()->getString(_symbol_entry->st_name) << " not found." << std::endl;
    return -1;
}

void SymbolTable::defineSymbol(Elf32_Sym* _symbol_entry, Elf32_Addr _value) {
    _symbol_entry->st_value = _value;
    _symbol_entry->st_defined = true;
}

void SymbolTable::print(std::ofstream& _file) const {
    _file << std::endl << "Symbol Table:" << std::endl;
    _file << "  ";
    _file << std::left << std::setfill(' ');
    _file << std::setw(4) << "NUM";
    _file << std::setw(25) << "NAME";
    _file << std::setw(9) << "VALUE";
    _file << std::setw(9) << "SIZE";
    _file << std::setw(9) << "TYPE";
    _file << std::setw(6) << "BIND";
    _file << std::setw(8) << "SHINDEX";
    _file << "DEFINED";
    _file << std::endl;
    uint32_t i = 0;
    for (Elf32_Sym* c : content) {
        std::string bind, type, section_index;
        switch (c->st_shndx) {
            case SHN_UNDEF:
                section_index = "UND";
                break;
            case SHN_ABS:
                section_index = "ABS";
                break;
            default:
                section_index = std::to_string(c->st_shndx);
                break;
        }

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
        switch (ELF32_ST_TYPE(c->st_info)) {
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
        _file << "  ";
        _file << std::right << std::setfill(' ') << std::dec;
        _file << std::setw(3) << i << " ";
        _file << std::left;
        _file << std::setw(24) << sht->getStringTable()->getString(c->st_name) << " ";
        _file << std::right << std::setfill('0') << std::hex;
        _file << std::setw(8) << c->st_value << " ";
        _file << std::setw(8) << c->st_size << " ";
        _file << std::setfill(' ') << std::dec << std::left;
        _file << std::setw(8) << type << " ";
        _file << std::setw(5) << bind << " ";
        _file << std::setw(7) << section_index << " ";
        _file << (c->st_defined ? "true" : "false");
        _file << std::endl;
        i += 1;
    }
}

void SymbolTable::write(std::ofstream* _file) {
    section_header->sh_size = content.size() * sizeof(Elf32_Sym);

    if (_file->tellp() % section_header->sh_addralign != 0) {
        _file->write("\0", section_header->sh_addralign - (_file->tellp() % section_header->sh_addralign));
    }

    section_header->sh_offset = _file->tellp();

    for (Elf32_Sym* symbol_entry : content) {
        _file->write((char*) symbol_entry, sizeof(Elf32_Sym));
    }
}
