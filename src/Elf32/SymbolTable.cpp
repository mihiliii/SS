#include "Elf32/SymbolTable.hpp"

#include "Elf32/Elf32.hpp"
#include "Elf32/Elf32File.hpp"

#include <algorithm>
#include <iomanip>

SymbolTable::SymbolTable(Elf32File& elf32_file)
    : Section(elf32_file,
              {.sh_type = SHT_SYMTAB, .sh_addralign = 4, .sh_entsize = sizeof(Elf32_Sym)}),
      _symbol_table()
{
    _header.sh_name = _elf32_file->string_table.add_string(".symtab");
}

Elf32_Sym& SymbolTable::add_symbol(const std::string& name, Elf32_Sym symbol_entry)
{
    if (_elf32_file->string_table.get_offset(name) == 0) {
        symbol_entry.st_name = _elf32_file->string_table.add_string(name);
    }
    else {
        symbol_entry.st_name = _elf32_file->string_table.get_offset(name);
    }

    _header.sh_size += sizeof(Elf32_Sym);
    return _symbol_table.emplace_back(symbol_entry);
}

Elf32_Sym& SymbolTable::add_symbol(const std::string& name, Elf32_Addr value, bool defined,
                                   Elf32_Half section_index, unsigned char info)
{
    Elf32_Off name_offset = _elf32_file->string_table.get_offset(name);
    if (name_offset == 0) {
        name_offset = _elf32_file->string_table.add_string(name);
    }

    Elf32_Sym symbol_entry = Elf32_Sym({.st_name = name_offset,
                                        .st_info = info,
                                        .st_shndx = section_index,
                                        .st_value = value,
                                        .st_defined = defined});

    _header.sh_size += sizeof(Elf32_Sym);
    _symbol_table.push_back(symbol_entry);
    return _symbol_table.back();
}

Elf32_Sym* SymbolTable::find_symbol(const std::string& name)
{
    for (Elf32_Sym& symbol : _symbol_table) {
        if (_elf32_file->string_table.get_string(symbol.st_name) == name) {
            return &symbol;
        }
    }
    return nullptr;
}

Elf32_Sym* SymbolTable::find_symbol(Elf32_Word entry_index)
{
    return &_symbol_table.at(entry_index);
}

Elf32_Sym& SymbolTable::get_symbol(const std::string& name)
{
    Elf32_Sym* symbol = find_symbol(name);
    if (symbol == nullptr) {
        throw std::runtime_error("Error: Symbol not found: " + name);
    }
    return *symbol;
}

Elf32_Sym& SymbolTable::get_symbol(Elf32_Word entry_index)
{
    if (entry_index >= _symbol_table.size()) {
        throw std::runtime_error("Error: Symbol index out of range: " +
                                 std::to_string(entry_index));
    }
    return _symbol_table.at(entry_index);
}

std::deque<Elf32_Sym>& SymbolTable::get_symbol_table()
{
    return _symbol_table;
}

void SymbolTable::set_symbol(Elf32_Sym& table_entry, const std::string& symbol_name,
                             Elf32_Byte st_info, Elf32_Half st_shndx, Elf32_Addr st_value,
                             bool st_defined)
{
    table_entry.st_name = _elf32_file->string_table.get_offset(symbol_name);
    if (table_entry.st_name == 0) {
        table_entry.st_name = _elf32_file->string_table.add_string(symbol_name);
    }

    table_entry = {table_entry.st_name, st_info, st_shndx, st_value, st_defined};
}

void SymbolTable::set_symbol(Elf32_Sym& table_entry, const std::string& symbol_name,
                             Elf32_Sym symbol)
{
    table_entry = symbol;

    table_entry.st_name = _elf32_file->string_table.get_offset(symbol_name);
    if (table_entry.st_name == 0) {
        table_entry.st_name = _elf32_file->string_table.add_string(symbol_name);
    }
}

void SymbolTable::set_symbol_table(const std::vector<Elf32_Sym>& symbol_table)
{
    _symbol_table.clear();
    for (const Elf32_Sym& symbol_entry : symbol_table) {
        _symbol_table.push_back(symbol_entry);
    }
    _header.sh_size = _symbol_table.size() * sizeof(Elf32_Sym);
}

void SymbolTable::sort()
{
    // Sort the symbol table by type (STT_SECTION, STT_NOTYPE) and then by binding (STB_LOCAL,
    // STB_GLOBAL, STB_WEAK)
    std::sort(_symbol_table.begin(), _symbol_table.end(),
              [](const Elf32_Sym& a, const Elf32_Sym& b) {
                  if (ELF32_ST_TYPE(a.st_info) == STT_SECTION &&
                      ELF32_ST_TYPE(b.st_info) != STT_SECTION) {
                      return true;
                  }
                  else if (ELF32_ST_TYPE(a.st_info) != STT_SECTION &&
                           ELF32_ST_TYPE(b.st_info) == STT_SECTION) {
                      return false;
                  }
                  else if (ELF32_ST_BIND(a.st_info) == STB_LOCAL &&
                           ELF32_ST_BIND(b.st_info) != STB_LOCAL) {
                      return true;
                  }
                  else if (ELF32_ST_BIND(a.st_info) != STB_LOCAL &&
                           ELF32_ST_BIND(b.st_info) == STB_LOCAL) {
                      return false;
                  }
                  else {
                      return a.st_value < b.st_value;
                  }
              });
}

Elf32_Word SymbolTable::get_symbol_index(const std::string& name)
{
    for (size_t i = 0; i < _symbol_table.size(); i++) {
        if (_elf32_file->string_table.get_string(_symbol_table[i].st_name) == name) {
            return i;
        }
    }
    return -1;
}

Elf32_Word SymbolTable::get_symbol_index(Elf32_Sym& symbol_entry)
{
    return get_symbol_index(_elf32_file->string_table.get_string(symbol_entry.st_name));
}

void SymbolTable::define_symbol(Elf32_Sym& symbol_entry, Elf32_Addr value, Elf32_Half section_index)
{
    symbol_entry.st_value = value;
    symbol_entry.st_defined = true;
    symbol_entry.st_shndx = section_index;
}

void SymbolTable::write(std::ostream& ostream)
{
    _header.sh_size = _symbol_table.size() * sizeof(Elf32_Sym);

    ostream.write("\0", _header.sh_addralign - (ostream.tellp() % _header.sh_addralign));

    _header.sh_offset = ostream.tellp();

    for (Elf32_Sym& symbol_entry : _symbol_table) {
        ostream.write((char*) &symbol_entry, sizeof(Elf32_Sym));
    }
}

void SymbolTable::print(std::ostream& ostream) const
{
    std::ostream os_copy(ostream.rdbuf());
    os_copy << std::endl << "Symbol Table:" << std::endl;
    os_copy << "  ";
    os_copy << std::left << std::setfill(' ');
    os_copy << std::setw(4) << "NUM";
    os_copy << std::setw(25) << "NAME";
    os_copy << std::setw(9) << "VALUE";
    os_copy << std::setw(9) << "TYPE";
    os_copy << std::setw(6) << "BIND";
    os_copy << std::setw(8) << "SHINDEX";
    os_copy << "DEFINED";
    os_copy << std::endl;
    for (size_t i = 0; i < _symbol_table.size(); i++) {
        const Elf32_Sym& symbol_entry = _symbol_table[i];
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
        os_copy << "  ";
        os_copy << std::right << std::setfill(' ') << std::dec;
        os_copy << std::setw(3) << i << " ";
        os_copy << std::left;
        os_copy << std::setw(24) << _elf32_file->string_table.get_string(symbol_entry.st_name)
                << " ";
        os_copy << std::right << std::setfill('0') << std::hex;
        os_copy << std::setw(8) << symbol_entry.st_value << " ";
        os_copy << std::setfill(' ') << std::dec << std::left;
        os_copy << std::setw(8) << type << " ";
        os_copy << std::setw(5) << bind << " ";
        os_copy << std::setw(7) << section_index << " ";
        os_copy << (symbol_entry.st_defined ? "true" : "false");
        os_copy << std::endl;
    }
}
