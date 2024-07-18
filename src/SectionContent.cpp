#include "../inc/SectionContent.hpp"


void SectionContent::writeContent(const std::string& _content) {
    content->append(_content);
    section_table_entry.sh_size += _content.length();
}

void SectionContent::printSectionTableEntry() {
    std::cout << "Section table entry:\n";
    std::cout << "Name: " << section_table_entry.sh_name << std::endl;
    std::cout << "Type: " << section_table_entry.sh_type << std::endl;
    std::cout << "Flags: " << section_table_entry.sh_flags << std::endl;
    std::cout << "Address: " << section_table_entry.sh_addr << std::endl;
    std::cout << "Offset: " << section_table_entry.sh_offset << std::endl;
    std::cout << "Size: " << section_table_entry.sh_size << std::endl;
    std::cout << "Link: " << section_table_entry.sh_link << std::endl;
    std::cout << "Info: " << section_table_entry.sh_info << std::endl;
    std::cout << "Address alignment: " << section_table_entry.sh_addralign << std::endl;
    std::cout << "Entry size: " << section_table_entry.sh_entsize << std::endl;
}

void SectionContent::printContent() { std::cout << "Content: " << *content << std::endl; }