#include "../inc/SymbolTable.hpp"

#include <algorithm>
#include <cstdint>
#include <iomanip>

#include "../inc/Elf32File.hpp"
#include "../inc/StringTable.hpp"
#include "../inc/misc/Exceptions.hpp"

const Elf32_Sym SymbolTable::kNullSymbol = {0, ELF32_ST_INFO(STB_LOCAL, STT_NOTYPE), 0, 0, 0, true};
const Elf32_Off SymbolTable::kNullSymbolIndex = 0;

SymbolTable::SymbolTable(Elf32File& elf32_file)
    : Section(elf32_file),
      _str_table(elf32_file.get_string_table()),
      _sym_table(std::deque<Elf32_Sym>({kNullSymbol}))
{
}

Elf32_Sym& SymbolTable::add_symbol(const std::string& name, Elf32_Sym symbol_entry)
{
    if (_str_table.get_string_offset(name) == 0) {
        symbol_entry.st_name = _str_table.add_string(name);
    } else {
        symbol_entry.st_name = _str_table.get_string_offset(name);
    }

    _header.sh_size = _sym_table.size() * sizeof(Elf32_Sym);
    return _sym_table.emplace_back(symbol_entry);
}

Elf32_Sym& SymbolTable::add_symbol(const std::string& name, Elf32_Addr value, bool defined,
                                   Elf32_Half section_index, Elf32_Byte section_info)
{
    Elf32_Off name_offset = _str_table.get_string_offset(name);
    if (name_offset == 0) {
        name_offset = _str_table.add_string(name);
    }

    Elf32_Sym symbol_entry = {name_offset, section_info, section_index, value, 0, defined};

    _header.sh_size = _sym_table.size() * sizeof(Elf32_Sym);
    return _sym_table.emplace_back(symbol_entry);
}

Elf32_Sym& SymbolTable::get_symbol(const std::string& name)
{
    for (auto& symbol : _sym_table) {
        if (_str_table.get_string(symbol.st_name) == name) {
            return symbol;
        }
    }
    return _sym_table.at(kNullSymbolIndex);
}

Elf32_Sym& SymbolTable::get_symbol(Elf32_Word entry_index)
{
    if (entry_index < _sym_table.size()) {
        return _sym_table.at(entry_index);
    } else {
        return _sym_table.at(kNullSymbolIndex);
    }
}

Elf32_Word SymbolTable::get_symbol_index(const std::string& name)
{
    for (Elf32_Word i = 0; i < _sym_table.size(); i++) {
        if (_str_table.get_string(_sym_table[i].st_name) == name) {
            return i;
        }
    }
    return kNullSymbolIndex;
}

Elf32_Word SymbolTable::get_symbol_index(const Elf32_Sym& symbol_entry)
{
    return get_symbol_index(_str_table.get_string(symbol_entry.st_name));
}

void SymbolTable::replace_data(const std::vector<Elf32_Sym>& symbol_table)
{
    _sym_table.clear();
    for (const Elf32_Sym& symbol_entry : symbol_table) {
        _sym_table.push_back(symbol_entry);
    }
    _header.sh_size = _sym_table.size() * sizeof(Elf32_Sym);
}

void SymbolTable::define_symbol(Elf32_Sym& symbol_entry, Elf32_Addr value, Elf32_Half section_index)
{
    if (symbol_entry.st_defined == true) {
        THROW_EXCEPTION("Symbol is already defined.");
    }

    symbol_entry.st_value = value;
    symbol_entry.st_shndx = section_index;
    symbol_entry.st_defined = true;
}

void SymbolTable::sort_data()
{
    // TODO: change the sorting algorithm
    std::sort(_sym_table.begin(), _sym_table.end(), [](const Elf32_Sym& a, const Elf32_Sym& b) {
        if (ELF32_ST_TYPE(a.st_info) > ELF32_ST_TYPE(b.st_info)) {
            return true;
        } else if (ELF32_ST_BIND(a.st_info) < ELF32_ST_BIND(b.st_info)) {
            return true;
        } else {
            return a.st_value < b.st_value;
        }
    });
}

void SymbolTable::set_symbol(Elf32_Sym& symbol_entry, Elf32_Sym new_symbol)
{
    new_symbol.st_name = symbol_entry.st_name;
    symbol_entry = new_symbol;
}

void SymbolTable::set_symbol(Elf32_Word symbol_index, Elf32_Sym new_symbol)
{
    if (symbol_index >= _sym_table.size()) {
        THROW_EXCEPTION("Symbol index out of range.");
    }

    new_symbol.st_name = _sym_table[symbol_index].st_name;
    _sym_table[symbol_index] = new_symbol;
}

void SymbolTable::print(std::ostream& ostream) const
{
    // clang-format off
    ostream << std::endl
             << "Symbol Table:" << std::endl
             << "  " 
             << std::left << std::setfill(' ') 
             << std::setw(4) << "NUM" 
             << std::setw(25) << "NAME"
             << std::setw(9) << "VALUE" 
             << std::setw(9) << "SIZE" 
             << std::setw(9) << "TYPE" 
             << std::setw(6) << "BIND"
             << std::setw(8) << "SHINDEX"
             << "DEFINED" << std::endl;

    uint32_t num = 0;
    for (const Elf32_Sym& symbol_entry : _sym_table) {
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
            default:
                type = "UNK";
                break;
        }
        ostream << "  " 
                 << std::right << std::setfill(' ') << std::dec 
                 << std::setw(3) << num 
                 << " " << std::left << std::setw(24) << _str_table.get_string(symbol_entry.st_name) 
                 << " " << std::right << std::setfill('0') << std::hex << std::setw(8) << symbol_entry.st_value 
                 << " " << std::setw(8) << symbol_entry.st_size
                 << " " << std::setfill(' ') << std::dec << std::left << std::setw(8) << type
                 << " " << std::setw(5) << bind
                 << " " << std::setw(7) << section_index 
                 << " " << (symbol_entry.st_defined ? "true" : "false") << std::endl;
        num += 1;
        // clang-format on
    }
}

void SymbolTable::write(std::ofstream& file)
{
    _header.sh_size = _sym_table.size() * sizeof(Elf32_Sym);

    file.write("\0", _header.sh_addralign - (file.tellp() % _header.sh_addralign));

    _header.sh_offset = file.tellp();

    for (Elf32_Sym& symbol_entry : _sym_table) {
        file.write((char*) &symbol_entry, sizeof(Elf32_Sym));
    }
}