#include "../../inc/Elf32/CustomSection.hpp"

#include "../../inc/Elf32/Elf32File.hpp"

#include <cstdint>
#include <iomanip>
#include <ostream>

CustomSection::CustomSection(Elf32File& elf32_file, const std::string& name)
    : Section(elf32_file),
      _rela_table(nullptr)
{
    _header.sh_type = SHT_CUSTOM;
    _header.sh_entsize = 4;
    _header.sh_addralign = 4;
    _header.sh_name = _elf32_file.string_table.add_string(name);
}

CustomSection::CustomSection(Elf32File& elf32_file, const std::string& name,
                             Elf32_Shdr section_header, const std::vector<Elf32_Byte>& data)
    : Section(elf32_file, section_header),
      _section_content(data),
      _rela_table(nullptr)
{
    _header.sh_name = _elf32_file.string_table.add_string(name);
}

void CustomSection::append_data(void* content, size_t content_size)
{
    for (size_t i = 0; i < content_size; i++) {
        _section_content.push_back(((Elf32_Byte*) content + i)[i]);
    }
    _header.sh_size += sizeof(Elf32_Byte) * content_size;
}

void CustomSection::append_data(Elf32_Byte content)
{
    _section_content.push_back(content);
    _header.sh_size += sizeof(content);
};

void CustomSection::append_data(instruction_format_t instruction)
{
    for (size_t i = 0; i < sizeof(instruction); i++) {
        Elf32_Byte byte = (Elf32_Byte) ((instruction >> (8 * i)) & 0xFF);
        _section_content.push_back(byte);
    }
    _header.sh_size += sizeof(instruction);
}

void CustomSection::append_data(const std::vector<Elf32_Byte>& content)
{
    _section_content.insert(_section_content.end(), content.begin(), content.end());
    _header.sh_size += content.size();
}

void CustomSection::overwrite_data(void* content, size_t content_size, Elf32_Off offset)
{
    Elf32_Byte* char_content = (Elf32_Byte*) content;
    for (size_t i = 0; i < content_size; i++) {
        _section_content[offset + i] = char_content[i];
    }
}

void CustomSection::replace_data(std::vector<Elf32_Byte> content)
{
    _section_content = content;
    _header.sh_size = _section_content.size();
}

const std::vector<Elf32_Byte>& CustomSection::get_data() const
{
    return _section_content;
}

instruction_format CustomSection::get_instruction(size_t index) const
{
    return _section_content[index] | (_section_content[index + 1] << 8) |
           (_section_content[index + 2] << 16) | (_section_content[index + 3] << 24);
}

size_t CustomSection::get_size() const
{
    return _section_content.size();
}

RelocationTable& CustomSection::get_rela_table()
{
    if (_rela_table == nullptr) {
        const std::string rela_name = RelocationTable::NAME_PREFIX + get_name();
        _rela_table = _elf32_file.new_relocation_table(rela_name, *this);
    }
    return *_rela_table;
}

bool CustomSection::has_rela_table()
{
    return _rela_table != nullptr;
}

void CustomSection::set_rela_table(RelocationTable* relocation_table)
{
    _rela_table = relocation_table;
}

void CustomSection::write(std::ostream& ostream)
{
    _header.sh_size = _section_content.size();

    if (ostream.tellp() % _header.sh_addralign != 0) {
        ostream.write("\0", _header.sh_addralign - (ostream.tellp() % _header.sh_addralign));
    }

    _header.sh_offset = ostream.tellp();
    ostream.write((const char*) _section_content.data(), _section_content.size());
}

void CustomSection::print(std::ostream& ostream) const
{
    std::ostream os_hex(ostream.rdbuf());
    os_hex << std::endl << "Section content " << this->get_name() << ":";
    os_hex << std::hex << std::setfill('0');

    for (size_t cnt = 0; cnt < _section_content.size(); cnt++) {
        if (cnt % 16 == 0) {
            os_hex << "\n" << std::right << std::setw(8) << cnt << ":";
        }
        os_hex << " " << std::setw(2) << (uint32_t) _section_content[cnt];
    }

    os_hex << std::endl;
}
