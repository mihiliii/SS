#include "../inc/Section.hpp"

#include "../inc/Elf32File.hpp"

Section::Section(Elf32File& elf32_file)
    : _elf32_file(elf32_file),
      _header_index(elf32_file.get_section_header_table().size()),
      _header(elf32_file.get_section_header_table().emplace_back())
{
}

Section::Section(Elf32File& elf32_file, Elf32_Shdr& section_header)
    : _elf32_file(elf32_file),
      _header_index(elf32_file.get_section_header_table().size()),
      _header(section_header)
{
}

Elf32_Word Section::get_index() const
{
    return _header_index;
}

Elf32_Shdr Section::get_header() const
{
    return _header;
}

void Section::set_header(const Elf32_Shdr& header)
{
    _header = header;
}

Section::~Section()
{
}
