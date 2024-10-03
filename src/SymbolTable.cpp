#include "../inc/SymbolTable.hpp"

#include <iomanip>

#include "../inc/Elf32File.hpp"
#include "../inc/StringTable.hpp"

SymbolTable::SymbolTable(Elf32File* _elf32_file) : Section(_elf32_file), symbol_table() {}

Elf32_Sym& SymbolTable::add(const std::string& _name, Elf32_Sym _symbol_entry) {
    if (elf32_file->stringTable().get(_name) == 0) {
        _symbol_entry.st_name = elf32_file->stringTable().add(_name);
    } else {
        _symbol_entry.st_name = elf32_file->stringTable().get(_name);
    }

    section_header.sh_size += sizeof(Elf32_Sym);
    symbol_table.emplace_back(_symbol_entry);
    return symbol_table.back();
}

Elf32_Sym& SymbolTable::add(
    const std::string& _name, Elf32_Addr _value, bool _defined, Elf32_Half _section_index, unsigned char _info
) {
    Elf32_Off name = elf32_file->stringTable().get(_name);
    if (name == 0) {
        name = elf32_file->stringTable().add(_name);
    }

    Elf32_Sym symbol_entry = Elf32_Sym(
        {.st_name = name,
         .st_info = _info,
         .st_shndx = _section_index,
         .st_value = _value,
         .st_size = 0,
         .st_defined = _defined}
    );

    section_header.sh_size += sizeof(Elf32_Sym);
    symbol_table.push_back(symbol_entry);
    return symbol_table.back();
}

Elf32_Sym* SymbolTable::get(const std::string& _name) {
    for (Elf32_Sym& symbol : symbol_table) {
        if (elf32_file->stringTable().get(symbol.st_name) == _name) {
            return &symbol;
        }
    }
    return nullptr;
}

Elf32_Sym* SymbolTable::get(uint32_t _entry_index) {
    if (_entry_index < symbol_table.size()) {
        return &symbol_table.at(_entry_index);
    }
    return nullptr;
}

std::deque<Elf32_Sym>& SymbolTable::symbolTable() {
    return symbol_table;
}

void SymbolTable::replace(const std::vector<Elf32_Sym>& _symbol_table) {
    symbol_table.clear();
    for (const Elf32_Sym& symbol_entry : _symbol_table) {
        symbol_table.push_back(symbol_entry);
    }
    section_header.sh_size = symbol_table.size() * sizeof(Elf32_Sym);
}

uint32_t SymbolTable::getIndex(const std::string& _name) {
    for (uint32_t i = 0; i < symbol_table.size(); i++) {
        if (elf32_file->stringTable().get(symbol_table[i].st_name) == _name)
            return i;
    }
    return -1;
}

uint32_t SymbolTable::getIndex(Elf32_Sym& _symbol_entry) {
    return getIndex(elf32_file->stringTable().get(_symbol_entry.st_name));
}

void SymbolTable::defineSymbol(Elf32_Sym* _symbol_entry, Elf32_Addr _value) {
    _symbol_entry->st_value = _value;
    _symbol_entry->st_defined = true;
}

void SymbolTable::print(std::ostream& _ostream) const {
    _ostream << std::endl << "Symbol Table:" << std::endl;
    _ostream << "  ";
    _ostream << std::left << std::setfill(' ');
    _ostream << std::setw(4) << "NUM";
    _ostream << std::setw(25) << "NAME";
    _ostream << std::setw(9) << "VALUE";
    _ostream << std::setw(9) << "SIZE";
    _ostream << std::setw(9) << "TYPE";
    _ostream << std::setw(6) << "BIND";
    _ostream << std::setw(8) << "SHINDEX";
    _ostream << "DEFINED";
    _ostream << std::endl;
    uint32_t i = 0;
    for (const Elf32_Sym& symbol_entry : symbol_table) {
        std::string bind, type, section_index;
        switch (symbol_entry.st_shndx) {
            case SHN_UNDEF:
                section_index = "UND";
                break;
            case SHN_ABS:
                section_index = "ABS";
                break;
            default:
                section_index = std::to_string(symbol_entry.st_shndx);
                break;
        }

        switch (ELF32_ST_BIND(symbol_entry.st_info)) {
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
        switch (ELF32_ST_TYPE(symbol_entry.st_info)) {
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
        _ostream << "  ";
        _ostream << std::right << std::setfill(' ') << std::dec;
        _ostream << std::setw(3) << i << " ";
        _ostream << std::left;
        _ostream << std::setw(24) << elf32_file->stringTable().get(symbol_entry.st_name) << " ";
        _ostream << std::right << std::setfill('0') << std::hex;
        _ostream << std::setw(8) << symbol_entry.st_value << " ";
        _ostream << std::setw(8) << symbol_entry.st_size << " ";
        _ostream << std::setfill(' ') << std::dec << std::left;
        _ostream << std::setw(8) << type << " ";
        _ostream << std::setw(5) << bind << " ";
        _ostream << std::setw(7) << section_index << " ";
        _ostream << (symbol_entry.st_defined ? "true" : "false");
        _ostream << std::endl;
        i += 1;
    }
}

void SymbolTable::write(std::ofstream* _file) {
    section_header.sh_size = symbol_table.size() * sizeof(Elf32_Sym);

    _file->write("\0", section_header.sh_addralign - (_file->tellp() % section_header.sh_addralign));

    section_header.sh_offset = _file->tellp();

    for (Elf32_Sym& symbol_entry : symbol_table) {
        _file->write((char*) &symbol_entry, sizeof(Elf32_Sym));
    }
}
