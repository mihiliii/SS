#include "../inc/RelocationTable.hpp"

#include <iomanip>

#include "../inc/CustomSection.hpp"
#include "../inc/Elf32.hpp"
#include "../inc/Elf32File.hpp"
#include "../inc/Section.hpp"
#include "../inc/StringTable.hpp"
#include "../inc/SymbolTable.hpp"

RelocationTable::RelocationTable(Elf32File* _elf32_file, CustomSection* _linked_section)
    : Section(_elf32_file), linked_section(_linked_section), relocation_table() {
    const std::string& relocation_table_name = ".rela" + _linked_section->name();
    section_header.sh_name = _elf32_file->stringTable().add(relocation_table_name);
    section_header.sh_type = SHT_RELA;
    section_header.sh_entsize = sizeof(Elf32_Rela);
    section_header.sh_info = _linked_section->index();
    section_header.sh_link = 0;
    section_header.sh_addralign = 4;
    section_header.sh_size = 0;
    _linked_section->setRelocationTable(this);
    _elf32_file->relocationTableMap().insert(std::make_pair(relocation_table_name, this));
}

RelocationTable::RelocationTable(
    Elf32File* _elf32_file,
    CustomSection* _linked_section,
    Elf32_Shdr _section_header,
    const std::vector<Elf32_Rela>& _relocation_table)
    : Section(_elf32_file, _section_header),
      linked_section(_linked_section),
      relocation_table(_relocation_table) {
    const std::string& relocation_table_name = ".rela" + _linked_section->name();
    section_header.sh_name = _elf32_file->stringTable().add(relocation_table_name);
    _linked_section->setRelocationTable(this);
    _elf32_file->relocationTableMap().insert(std::make_pair(relocation_table_name, this));
    std::cout << _elf32_file->relocationTableMap().size() << std::endl;
}

void RelocationTable::print(std::ostream& _ostream) const {
    _ostream << std::endl << "Relocation table " << this->name() << ":" << std::endl;
    _ostream << std::left << std::setfill(' ');
    _ostream << "  ";
    _ostream << std::setw(4) << "NUM";
    _ostream << std::setw(9) << "OFFSET";
    _ostream << std::setw(9) << "TYPE";
    _ostream << std::setw(25) << "SYMBOL";
    _ostream << std::setw(9) << "ADDEND";
    _ostream << std::endl;
    int i = 0;
    for (Elf32_Rela relocation_table_entry : relocation_table) {
        Elf32_Half symbol_index = ELF32_R_SYM(relocation_table_entry.r_info);
        Elf32_Sym* symbol = elf32_file->symbolTable().get(symbol_index);

        std::string symbol_name = elf32_file->stringTable().get(symbol->st_name);
        std::string relocation_type;

        switch (ELF32_R_TYPE(relocation_table_entry.r_info)) {
            case ELF32_R_ABS32:
                relocation_type = "ABS32";
                break;
            default:
                break;
        }

        _ostream << std::right << std::dec << std::setfill(' ') << std::setw(5) << i++ << " ";
        _ostream << std::hex << std::setfill('0');
        _ostream << std::setw(8) << relocation_table_entry.r_offset << " ";
        _ostream << std::setw(8) << std::setfill(' ') << std::left << relocation_type << " ";
        _ostream << std::setw(25) << (std::to_string(symbol_index) + " (" + symbol_name + ")");
        _ostream << std::dec << std::left << std::setfill(' ');
        _ostream << std::setw(8) << relocation_table_entry.r_addend;
        _ostream << std::endl;
    }
}

void RelocationTable::write(std::ofstream* _file) {
    if (_file->tellp() % section_header.sh_addralign != 0) {
        _file->write(
            "\0", section_header.sh_addralign - (_file->tellp() % section_header.sh_addralign));
    }

    section_header.sh_offset = _file->tellp();
    section_header.sh_size = relocation_table.size() * sizeof(Elf32_Rela);

    for (Elf32_Rela& relocation_table_entry : relocation_table) {
        _file->write((char*) &relocation_table_entry, sizeof(Elf32_Rela));
    }
}

void RelocationTable::add(Elf32_Rela _rela_entry) {
    relocation_table.push_back(_rela_entry);
    section_header.sh_size += sizeof(Elf32_Rela);
}

void RelocationTable::add(Elf32_Addr _offset, Elf32_Word _info, Elf32_SWord _addend) {
    Elf32_Rela rela_entry = {.r_offset = _offset, .r_info = _info, .r_addend = _addend};
    relocation_table.push_back(rela_entry);
    section_header.sh_size += sizeof(Elf32_Rela);
}

void RelocationTable::add(const std::vector<Elf32_Rela>& _content) {
    relocation_table.insert(relocation_table.end(), _content.begin(), _content.end());
    section_header.sh_size += _content.size() * sizeof(Elf32_Rela);
}
