#include "../inc/SectionHeaderTable.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>

#include "../inc/Assembler.hpp"

SectionHeaderTable& SectionHeaderTable::getInstance() {
    static SectionHeaderTable instance;
    return instance;
}

uint32_t SectionHeaderTable::insert(Elf32_Shdr* _section_entry) {
    section_header_table.emplace_back(_section_entry);
    return section_header_table_index++;
}

void SectionHeaderTable::write(std::ofstream* _file) {
    for (Elf32_Shdr* section : section_header_table) {
        _file->write(reinterpret_cast<char*>(section), sizeof(Elf32_Shdr));
    }
}

void SectionHeaderTable::printSectionHeaderTable() {
    std::cout << "Section Header Table:" << std::endl;
    std::cout << "  ";
    std::cout << std::left << std::setfill(' ');
    std::cout << std::setw(4) << "NUM";
    std::cout << std::setw(16) << "NAME";
    std::cout << std::setw(5) << "TYPE";
    std::cout << std::setw(9) << "ADDRESS";
    std::cout << std::setw(9) << "OFFSET";
    std::cout << std::setw(9) << "SIZE";
    std::cout << std::setw(9) << "FLAGS";
    std::cout << std::setw(5) << "LINK";
    std::cout << std::setw(5) << "INFO";
    std::cout << std::setw(9) << "ALIGN";
    std::cout << std::setw(9) << "ENTSIZE";
    std::cout << std::endl;
    uint32_t index = 0;
    for (Elf32_Shdr* section : section_header_table) {
        std::cout << "  ";
        std::cout << std::setw(3) << std::right << std::dec << std::setfill(' ') << index << " ";
        std::cout << std::left;
        std::cout << std::setw(16) << StringTable::getInstance().getString(section->sh_name);
        std::cout << std::right << std::hex << std::setfill('0');
        std::cout << std::setw(4) << section->sh_type << " ";
        std::cout << std::setw(8) << section->sh_addr << " ";
        std::cout << std::setw(8) << section->sh_offset << " ";
        std::cout << std::setw(8) << section->sh_size << " ";
        std::cout << std::setw(8) << section->sh_flags << " ";
        std::cout << std::setw(4) << section->sh_link << " ";
        std::cout << std::setw(4) << section->sh_info << " ";
        std::cout << std::setw(8) << section->sh_addralign << " ";
        std::cout << std::setw(8) << section->sh_entsize << std::endl;
        index++;
    }
}

SectionHeaderTable::SectionHeaderTable() : section_header_table(), section_header_table_index(0) {}
