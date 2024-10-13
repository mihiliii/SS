#include "../inc/Section.hpp"

#include "../inc/Elf32File.hpp"

Section::Section(Elf32File* _elf32_file) : elf32_file(_elf32_file), sh_table_index(_elf32_file->sectionHeaderTable().size()) {
    _elf32_file->sectionHeaderTable().emplace_back();
}

Section::Section(Elf32File* _elf32_file, Elf32_Shdr _section_header)
    : elf32_file(_elf32_file),
      sh_table_index(_elf32_file->sectionHeaderTable().size()) {
    _elf32_file->sectionHeaderTable().emplace_back(_section_header);
}

std::string Section::name() const {
    const Elf32_Shdr& section_header = elf32_file->sectionHeaderTable()[sh_table_index];
    return elf32_file->stringTable().get(section_header.sh_name);
}

Elf32_Half Section::index() const {
    return sh_table_index;
}

Elf32_Shdr& Section::header() {
    return elf32_file->sectionHeaderTable()[sh_table_index];
}

Section::~Section() {}
