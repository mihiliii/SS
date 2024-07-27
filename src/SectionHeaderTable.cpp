#include "../inc/SectionHeaderTable.hpp"

#include <iostream>

#include "SectionHeaderTable.hpp"

SectionHeaderTable& SectionHeaderTable::getInstance() {
    static SectionHeaderTable instance;
    return instance;
}

void SectionHeaderTable::insertSectionEntry(Elf32_Shdr** _handle) {
    *_handle = &section_header_table.emplace_back(Elf32_Shdr());
}

void SectionHeaderTable::printSectionTable() {
    for (Elf32_Shdr section : section_header_table) {
        std::cout << "Section name: " << section.sh_name << std::endl;
        std::cout << "Section type: " << section.sh_type << std::endl;
        std::cout << "Section flags: " << section.sh_flags << std::endl;
        std::cout << "Section address: " << section.sh_addr << std::endl;
        std::cout << "Section offset: " << section.sh_offset << std::endl;
        std::cout << "Section size: " << section.sh_size << std::endl;
        std::cout << "Section link: " << section.sh_link << std::endl;
        std::cout << "Section info: " << section.sh_info << std::endl;
        std::cout << "Section addralign: " << section.sh_addralign << std::endl;
        std::cout << "Section entsize: " << section.sh_entsize << std::endl;
        std::cout << std::endl;
    }
};

