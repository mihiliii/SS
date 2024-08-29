#include "../inc/RelocationTable.hpp"

#include <iomanip>

#include "../inc/Assembler.hpp"
#include "../inc/CustomSection.hpp"
#include "../inc/Elf32.hpp"
#include "../inc/Section.hpp"
#include "../inc/StringTable.hpp"

RelocationTable::RelocationTable(CustomSection* _linked_section) : Section(), parent_section(_linked_section) {
    section_header.sh_name = Assembler::string_table->addString(std::string(".rela") + _linked_section->getName());
    section_header.sh_type = SHT_RELA;
    section_header.sh_entsize = sizeof(Elf32_Rela);
    section_header.sh_info = _linked_section->getSectionHeaderTableIndex();
    section_header.sh_link = Assembler::symbol_table->getSectionHeaderTableIndex();
    section_header.sh_addralign = 4;
    section_header.sh_size = 0;
}

void RelocationTable::print() const {
    std::cout << "Relocation table " << this->getName() << ":" << std::endl;
    std::cout << std::left << std::setfill(' ');
    std::cout << "  ";
    std::cout << std::setw(4) << "NUM";
    std::cout << std::setw(9) << "OFFSET";
    std::cout << std::setw(9) << "TYPE";
    std::cout << std::setw(25) << "SYMBOL";
    std::cout << std::setw(9) << "ADDEND";
    std::cout << std::endl;
    int i = 0;
    for (Elf32_Rela relocation_table_entry : relocation_table) {
        Elf32_Half symbol_index = ELF32_R_SYM(relocation_table_entry.r_info);
        std::string symbol_name = Section::getName(symbol_index);
        std::string relocation_type;

        switch (ELF32_R_TYPE(relocation_table_entry.r_info)) {
            case R_ABS32:
                relocation_type = "ABS32";
                break;
            default:
                break;
        }

        std::cout << std::right << std::dec << std::setfill(' ') << std::setw(5) << i++ << " ";
        std::cout << std::hex << std::setfill('0');
        std::cout << std::setw(8) << relocation_table_entry.r_offset << " ";
        std::cout << std::setw(8) << std::setfill(' ') << std::left << relocation_type << " ";
        std::cout << std::setw(25) << (std::to_string(symbol_index) + " (" + symbol_name + ")");
        std::cout << std::dec << std::left << std::setfill(' ');
        std::cout << std::setw(8) << relocation_table_entry.r_addend;
        std::cout << std::endl;
    }
}

void RelocationTable::write(std::ofstream* _file) {
    if (_file->tellp() % section_header.sh_addralign != 0) {
        _file->write("\0", section_header.sh_addralign - (_file->tellp() % section_header.sh_addralign));
    }

    section_header.sh_offset = _file->tellp();

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
