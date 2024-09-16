#include "../inc/SymbolTable.hpp"

#include <iomanip>

#include "../inc/Elf32File.hpp"
#include "../inc/StringTable.hpp"
#include "SymbolTable.hpp"

SymbolTable::SymbolTable(Elf32File* _elf32_file) : Section(_elf32_file), symbol_table() {}

SymbolTable::SymbolTable(Elf32File* _elf32_file, Elf32_Shdr _section_header, std::vector<Elf32_Sym> _symbol_table)
    : Section(_elf32_file, _section_header), symbol_table() {
    for (auto& symbol_entry : _symbol_table) {
        symbol_table.emplace_back(new Elf32_Sym(symbol_entry));
    }
}

Elf32_Sym* SymbolTable::add(std::string _name, Elf32_Sym _symbol_entry) {
    Elf32_Sym* symbol_entry = new Elf32_Sym(_symbol_entry);
    if ((int) elf32_file->getStringTable().get(_name) == 0)
        symbol_entry->st_name = elf32_file->getStringTable().add(_name);
    else {
        std::cout << "Warning: Symbol name" << _name << " already exists in String Table" << std::endl;
        symbol_entry->st_name = elf32_file->getStringTable().get(_name);
    }

    symbol_table.emplace_back(symbol_entry);
    section_header.sh_size += sizeof(Elf32_Sym);
    return symbol_entry;
}

Elf32_Sym* SymbolTable::add(
    std::string _name, Elf32_Addr _value, bool _defined, Elf32_Half _section_index, unsigned char _info
) {
    Elf32_Sym* symbol_entry = new Elf32_Sym(
        {.st_name = elf32_file->getStringTable().add(_name),
         .st_info = _info,
         .st_shndx = _section_index,
         .st_value = _value,
         .st_size = 0,
         .st_defined = _defined}
    );

    symbol_table.emplace_back(symbol_entry);
    section_header.sh_size += sizeof(Elf32_Sym);
    return symbol_entry;
}

Elf32_Sym* SymbolTable::get(std::string _name) {
    for (Elf32_Sym* symbol : symbol_table) {
        if (elf32_file->getStringTable().get(symbol->st_name) == _name)
            return symbol;
    }
    return nullptr;
}

Elf32_Sym* SymbolTable::get(uint32_t _entry_index) {
    if (_entry_index >= symbol_table.size())
        return nullptr;
    return symbol_table.at(_entry_index);
}

std::vector<Elf32_Sym*>& SymbolTable::getContent() {
    return symbol_table;
}

uint32_t SymbolTable::getIndex(std::string _name) {
    for (uint32_t i = 0; i < symbol_table.size(); i++) {
        if (elf32_file->getStringTable().get(symbol_table[i]->st_name) == _name)
            return i;
    }
    return -1;
}

uint32_t SymbolTable::getIndex(Elf32_Sym* _symbol_entry) {
    for (uint32_t i = 0; i < symbol_table.size(); i++) {
        if (symbol_table[i] == _symbol_entry)
            return i;
    }
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
    for (Elf32_Sym* c : symbol_table) {
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
        _file << std::setw(24) << elf32_file->getStringTable().get(c->st_name) << " ";
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
    section_header.sh_size = symbol_table.size() * sizeof(Elf32_Sym);

    _file->write("\0", section_header.sh_addralign - (_file->tellp() % section_header.sh_addralign));

    section_header.sh_offset = _file->tellp();

    for (Elf32_Sym* symbol_entry : symbol_table) {
        _file->write((char*) symbol_entry, sizeof(Elf32_Sym));
    }
}

SymbolTable::~SymbolTable() {
    for (Elf32_Sym* symbol_entry : symbol_table) {
        delete symbol_entry;
    }
    symbol_table.clear();
}
