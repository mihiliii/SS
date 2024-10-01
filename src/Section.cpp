#include "../inc/Section.hpp"

#include "../inc/Elf32File.hpp"

Section::Section(Elf32File* _elf32_file)
    : elf32_file(_elf32_file), section_header(), sh_table_index(_elf32_file->sectionHeaderTable().size()) {
    _elf32_file->sectionHeaderTable().push_back(&section_header);
}

Section::Section(Elf32File* _elf32_file, Elf32_Shdr _section_header)
    : elf32_file(_elf32_file),
      section_header(_section_header),
      sh_table_index(_elf32_file->sectionHeaderTable().size()) {
    _elf32_file->sectionHeaderTable().push_back(&section_header);
}

std::string Section::name() const {
    return elf32_file->stringTable().get(section_header.sh_name);
}

Section::~Section() {
    elf32_file->sectionHeaderTable().erase(elf32_file->sectionHeaderTable().begin() + sh_table_index);
}

Elf32_Half Section::index() const {
    return sh_table_index;
}

Elf32_Shdr& Section::header() {
    return section_header;
}
