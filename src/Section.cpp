#include "../inc/Section.hpp"

#include "../inc/Elf32File.hpp"

Section::Section(Elf32File* _elf32_file)
    : elf32_file(_elf32_file), section_header(), sh_table_index(_elf32_file->getSectionHeaderTable().size()) {}

Section::Section(Elf32File* _elf32_file, Elf32_Shdr _section_header)
    : elf32_file(_elf32_file),
      section_header(_section_header),
      sh_table_index(_elf32_file->getSectionHeaderTable().size()) {}

std::string Section::name() const {
    return elf32_file->getStringTable().get(section_header.sh_name);
}

Elf32_Half Section::index() const {
    return sh_table_index;
}

Elf32_Shdr& Section::header() {
    return section_header;
}
