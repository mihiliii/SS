#ifndef SectionHeaderTable_hpp_
#define SectionHeaderTable_hpp_

#include "Elf32.hpp"
#include <vector>

class SectionHeaderTable {
public:

    SectionHeaderTable() : section_header_table({}) {}

    ~SectionHeaderTable() {}

    void insertSectionEntry(Elf32_Shdr section_entry) { section_header_table.push_back(section_entry); }

    void printSectionTable() {
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

    std::vector<Elf32_Shdr>& getSectionTable() { return section_header_table; }

private:

    std::vector<Elf32_Shdr> section_header_table;
};

#endif