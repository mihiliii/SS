#include "../../inc/Elf32/Section.hpp"

#include "../../inc/Elf32/Elf32File.hpp"

Section::Section(Elf32File& elf32_file)
    : _elf32_file(elf32_file),
      _header(Elf32_Shdr()),
      _header_index(elf32_file._section_header_table.size())
{
    _elf32_file._section_header_table.emplace_back(_header);
}

Section::Section(Elf32File& elf32_file, Elf32_Shdr _section_header)
    : _elf32_file(elf32_file),
      _header(_section_header),
      _header_index(elf32_file._section_header_table.size())
{
    _elf32_file._section_header_table.emplace_back(_section_header);
}

Elf32_Shdr Section::get_header() const
{
    return _elf32_file._section_header_table[_header_index];
}

Elf32_Word Section::get_index() const
{
    return _header_index;
}

const std::string& Section::get_name() const
{
    const Elf32_Shdr& section_header = _elf32_file._section_header_table[_header_index];
    return _elf32_file._string_table.get_string(section_header.sh_name);
}

void Section::set_header(Elf32_Shdr header)
{
    _header = header;
}
