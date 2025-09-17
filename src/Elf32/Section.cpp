#include "Elf32/Section.hpp"

#include "Elf32/Elf32File.hpp"

Section::Section(Elf32File& elf32_file)
    : _elf32_file(&elf32_file),
      _header(Elf32_Shdr()),
      _header_index(elf32_file.section_header_table.size())
{
    _elf32_file->section_header_table.emplace_back(&_header);
}

Section::Section(Elf32File& elf32_file, Elf32_Shdr _section_header)
    : _elf32_file(&elf32_file),
      _header(_section_header),
      _header_index(elf32_file.section_header_table.size())
{
    _elf32_file->section_header_table.emplace_back(&_header);
}

Section::Section(Section&& other)
    : _elf32_file(other._elf32_file),
      _header(other._header),
      _header_index(other._header_index)
{
    _elf32_file->section_header_table[_header_index] = &_header;
    other._elf32_file = nullptr;
}

Section& Section::operator=(Section&& other)
{
    if (this != &other) {
        _elf32_file = other._elf32_file;
        _header = other._header;
        _header_index = other._header_index;

        _elf32_file->section_header_table[_header_index] = &_header;

        other._elf32_file = nullptr;
    }
    return *this;
}

Elf32_Shdr Section::get_header() const
{
    return _header;
}

Elf32_Word Section::get_header_index() const
{
    return _header_index;
}

const std::string& Section::get_name() const
{
    return _elf32_file->string_table.get_string(_header.sh_name);
}

void Section::set_header(Elf32_Shdr header)
{
    _header = header;
}

Section::~Section() {}
