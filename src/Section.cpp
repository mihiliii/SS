#include "../inc/Section.hpp"

#include "../inc/Elf32_File.hpp"

Section::Section(Elf32_File* _elf32_file)
    : elf32_file(_elf32_file), section_header(), sh_table_index(_elf32_file->getSectionHeaderTable().size()) {}

Section::Section(Elf32_File* _elf32_file, Elf32_Shdr _section_header)
    : elf32_file(_elf32_file),
      section_header(_section_header),
      sh_table_index(_elf32_file->getSectionHeaderTable().size()) {}

std::string Section::getName() const {
    return elf32_file->getStringTable().get(section_header.sh_name);
}

Elf32_Half Section::getIndex() const {
    return sh_table_index;
}

Elf32_Shdr& Section::getHeader() {
    return section_header;
}
