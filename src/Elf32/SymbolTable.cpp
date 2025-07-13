#include "../../inc/Elf32/SymbolTable.hpp"

#include <algorithm>
#include <iomanip>

#include "../../inc/Elf32/Elf32File.hpp"
#include "../../inc/Elf32/StringTable.hpp"
#include "Elf32/Elf32.hpp"

SymbolTable::SymbolTable(Elf32File& elf32_file) : Section(elf32_file), _symbol_table() {}

Elf32_Sym& SymbolTable::add_symbol(const std::string& name, Elf32_Sym symbol_entry) {
    if (_elf32_file.stringTable().get_offset(name) == 0) {
        symbol_entry.st_name = _elf32_file.stringTable().add_string(name);
    } else {
        symbol_entry.st_name = _elf32_file.stringTable().get_offset(name);
    }

    _header.sh_size += sizeof(Elf32_Sym);
    _symbol_table.emplace_back(symbol_entry);
    return _symbol_table.back();
}

Elf32_Sym& SymbolTable::add_symbol(const std::string& name, Elf32_Addr value, bool defined,
                                   Elf32_Half section_index, unsigned char info) {
    // TODO: CHECK IF GOOD
    Elf32_Off offset = _elf32_file.stringTable().get_offset(name);
    if (offset == 0) {
        offset = _elf32_file.stringTable().add_string(name);
    }

    Elf32_Sym symbol_entry = Elf32_Sym({.st_name = offset,
                                        .st_info = info,
                                        .st_shndx = section_index,
                                        .st_value = value,
                                        .st_size = 0,
                                        .st_defined = defined});

    _header.sh_size += sizeof(Elf32_Sym);
    _symbol_table.push_back(symbol_entry);
    return _symbol_table.back();
}

Elf32_Sym* SymbolTable::get_symbol(const std::string& name) {
    for (Elf32_Sym& symbol : _symbol_table) {
        if (_elf32_file.stringTable().get_string(symbol.st_name) == name) {
            return &symbol;
        }
    }
    return nullptr;
}

Elf32_Sym* SymbolTable::get_symbol(Elf32_Word entry_index) {
    if (entry_index < _symbol_table.size()) {
        return &_symbol_table.at(entry_index);
    }
    return nullptr;
}

std::deque<Elf32_Sym>& SymbolTable::get_symbol_table() { return _symbol_table; }

void SymbolTable::replace_table(const std::vector<Elf32_Sym>& symbol_table) {
    _symbol_table.clear();
    for (const Elf32_Sym& symbol_entry : _symbol_table) {
        _symbol_table.push_back(symbol_entry);
    }
    _header.sh_size = _symbol_table.size() * sizeof(Elf32_Sym);
}

void SymbolTable::change_values(Elf32_Sym& old_symbol, Elf32_Sym new_symbol) {
    new_symbol.st_name = old_symbol.st_name;
    old_symbol = new_symbol;
}

void SymbolTable::sort() {
    // Sort the symbol table by type (STT_SECTION, STT_NOTYPE) and then by binding (STB_LOCAL,
    // STB_GLOBAL, STB_WEAK)
    std::sort(_symbol_table.begin(), _symbol_table.end(),
              [](const Elf32_Sym& a, const Elf32_Sym& b) {
                  if (ELF32_ST_TYPE(a.st_info) == STT_SECTION &&
                      ELF32_ST_TYPE(b.st_info) != STT_SECTION) {
                      return true;
                  } else if (ELF32_ST_TYPE(a.st_info) != STT_SECTION &&
                             ELF32_ST_TYPE(b.st_info) == STT_SECTION) {
                      return false;
                  } else if (ELF32_ST_BIND(a.st_info) == STB_LOCAL &&
                             ELF32_ST_BIND(b.st_info) != STB_LOCAL) {
                      return true;
                  } else if (ELF32_ST_BIND(a.st_info) != STB_LOCAL &&
                             ELF32_ST_BIND(b.st_info) == STB_LOCAL) {
                      return false;
                  } else {
                      return a.st_value < b.st_value;
                  }
              });
}

Elf32_Word SymbolTable::get_index(const std::string& name) {
    for (uint32_t i = 0; i < _symbol_table.size(); i++) {
        if (_elf32_file.stringTable().get_string(_symbol_table[i].st_name) == name) {
            return i;
        }
    }
    return -1;
}

uint32_t SymbolTable::get_index(Elf32_Sym& symbol_entry) {
    return get_index(_elf32_file.stringTable().get_string(symbol_entry.st_name));
}

void SymbolTable::define_symbol(Elf32_Sym* symbol_entry, Elf32_Addr value,
                                Elf32_Half section_index) {
    symbol_entry->st_value = value;
    symbol_entry->st_defined = true;
    symbol_entry->st_shndx = section_index;
}

void SymbolTable::print(std::ostream& ostream) const {
    ostream << std::endl << "Symbol Table:" << std::endl;
    ostream << "  ";
    ostream << std::left << std::setfill(' ');
    ostream << std::setw(4) << "NUM";
    ostream << std::setw(25) << "NAME";
    ostream << std::setw(9) << "VALUE";
    ostream << std::setw(9) << "SIZE";
    ostream << std::setw(9) << "TYPE";
    ostream << std::setw(6) << "BIND";
    ostream << std::setw(8) << "SHINDEX";
    ostream << "DEFINED";
    ostream << std::endl;
    uint32_t i = 0;
    for (const Elf32_Sym& symbol_entry : _symbol_table) {
        std::string bind, type, section_index;
        switch (symbol_entry.st_shndx) {
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
        ostream << "  ";
        ostream << std::right << std::setfill(' ') << std::dec;
        ostream << std::setw(3) << i << " ";
        ostream << std::left;
        ostream << std::setw(24) << _elf32_file.stringTable().get_string(symbol_entry.st_name)
                << " ";
        ostream << std::right << std::setfill('0') << std::hex;
        ostream << std::setw(8) << symbol_entry.st_value << " ";
        ostream << std::setw(8) << symbol_entry.st_size << " ";
        ostream << std::setfill(' ') << std::dec << std::left;
        ostream << std::setw(8) << type << " ";
        ostream << std::setw(5) << bind << " ";
        ostream << std::setw(7) << section_index << " ";
        ostream << (symbol_entry.st_defined ? "true" : "false");
        ostream << std::endl;
        i += 1;
    }
}

void SymbolTable::write(std::ofstream* file) {
    _header.sh_size = _symbol_table.size() * sizeof(Elf32_Sym);

    file->write("\0", _header.sh_addralign - (file->tellp() % _header.sh_addralign));

    _header.sh_offset = file->tellp();

    for (Elf32_Sym& symbol_entry : _symbol_table) {
        file->write((char*) &symbol_entry, sizeof(Elf32_Sym));
    }
}
