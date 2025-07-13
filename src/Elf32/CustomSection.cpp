#include "../../inc/Elf32/CustomSection.hpp"

#include "../../inc/Elf32/Elf32File.hpp"
#include <iomanip>

CustomSection::CustomSection(Elf32File& elf32_file, const std::string& name)
    : Section(elf32_file),
      _relocation_table(nullptr)
{
    _header.sh_type = SHT_CUSTOM;
    _header.sh_entsize = 4;
    _header.sh_addralign = 4;
    _header.sh_name = _elf32_file.stringTable().add_string(name);
}
CustomSection::CustomSection(Elf32File& elf32_file, const std::string& name,
                             Elf32_Shdr section_header, const std::vector<char>& data)
    : Section(elf32_file, section_header),
      _section_content(data),
      _relocation_table(nullptr)
{
    _header.sh_name = _elf32_file.stringTable().add_string(name);
}

void CustomSection::append(void* content, size_t content_size)
{
    char* char_content = (char*) content;
    for (size_t i = 0; i < content_size; i++) {
        _section_content.push_back(char_content[i]);
    }
    _header.sh_size += sizeof(char) * content_size;
}

void CustomSection::append(instruction_format_t content)
{
    uint32_t instruction = (uint32_t) content;
    for (size_t i = 0; i < sizeof(content); i++) {
        _section_content.push_back((char) ((instruction >> (8 * i)) & 0xFF));
    }
    _header.sh_size += sizeof(content);
}

void CustomSection::overwrite(void* content, size_t content_size, Elf32_Off offset)
{
    char* char_content = (char*) content;
    for (size_t i = 0; i < content_size; i++) {
        _section_content[offset + i] = char_content[i];
    }
}

char* CustomSection::content(Elf32_Off offset) { return &_section_content[offset]; }

std::vector<char>& CustomSection::content() { return _section_content; }

void CustomSection::replace(std::vector<char> content)
{
    _section_content = content;
    _header.sh_size = _section_content.size();
}

size_t CustomSection::size() const { return _section_content.size(); }

RelocationTable& CustomSection::relocationTable()
{
    if (_relocation_table == nullptr) {
        _relocation_table = _elf32_file.newRelocationTable(this);
    }
    return *_relocation_table;
}

bool CustomSection::hasRelocationTable() { return _relocation_table != nullptr; }

void CustomSection::setRelocationTable(RelocationTable* relocation_table)
{
    _relocation_table = relocation_table;
}

void CustomSection::print(std::ostream& ostream) const
{
    ostream << std::endl << "Content of section " << name() << ":\n";
    for (uint32_t location_counter = 0; location_counter < _section_content.size();
         location_counter++) {
        if (location_counter % 16 == 0) {
            ostream << std::right << std::hex << std::setw(8) << std::setfill('0')
                    << location_counter << ": ";
        }
        ostream << std::hex << std::setw(2) << std::setfill('0')
                << (unsigned int) (unsigned char) _section_content[location_counter] << " ";
        if ((location_counter + 1) % 16 == 0) {
            ostream << "\n";
        }
    }
    ostream << std::left << std::dec << "\n";
}

void CustomSection::write(std::ofstream* file)
{
    _header.sh_size = _section_content.size();

    if (file->tellp() % _header.sh_addralign != 0) {
        file->write("\0", _header.sh_addralign - (file->tellp() % _header.sh_addralign));
    }

    _header.sh_offset = file->tellp();
    file->write(_section_content.data(), _section_content.size());

    if (_relocation_table != nullptr) {
        _relocation_table->write(file);
    }
}
