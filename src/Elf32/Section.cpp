#include "../../inc/Elf32/Section.hpp"

#include "../../inc/Elf32/Elf32File.hpp"

Section::Section(Elf32File& elf32_file) :
    _elf32_file(elf32_file), _header_index(elf32_file.sectionHeaderTable().size()) {
    _elf32_file.sectionHeaderTable().emplace_back();
}

Section::Section(Elf32File& elf32_file, Elf32_Shdr _section_header) :
    _elf32_file(elf32_file), _header_index(elf32_file.sectionHeaderTable().size()) {
    _elf32_file.sectionHeaderTable().emplace_back(_section_header);
}

Elf32_Shdr& Section::get_header() { return _elf32_file.sectionHeaderTable()[_header_index]; }

const Elf32_Word Section::get_index() const { return _header_index; }

const std::string& Section::name() const {
    const Elf32_Shdr& section_header = _elf32_file.sectionHeaderTable()[_header_index];
    return _elf32_file.stringTable().get(section_header.sh_name);
}

Section::~Section() {}
