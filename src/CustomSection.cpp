#include "../inc/CustomSection.hpp"

#include <iomanip>
#include <iostream>

#include "../inc/Elf32File.hpp"
#include "../inc/RelocationTable.hpp"
#include "../inc/StringTable.hpp"
#include "../inc/SymbolTable.hpp"
#include "../inc/misc/Exceptions.hpp"

CustomSection::CustomSection(Elf32File& elf32_file, const std::string& name)
    : Section(elf32_file), _data(), _relocation_table(nullptr)
{
    _header.sh_type = SHT_CUSTOM;
    _header.sh_entsize = 4;
    _header.sh_addralign = 4;
    _header.sh_name = elf32_file.get_string_table().add_string(name);
}

CustomSection::CustomSection(Elf32File& elf32_file, const std::string& name,
                             Elf32_Shdr section_header, const std::vector<char>& data)
    : Section(elf32_file, section_header), _data(data), _relocation_table(nullptr)
{
    _header.sh_name = elf32_file.get_string_table().add_string(name);
}

void CustomSection::add_data(void* data, size_t data_size)
{
    if (data == nullptr) {
        THROW_EXCEPTION("Data to append is null.");
    }

    char* c_data = (char*) data;
    for (size_t i = 0; i < data_size; i++) {
        _data.push_back(c_data[i]);
    }
    _header.sh_size = sizeof(char) * _data.size();
}

void CustomSection::add_data(const std::vector<char>& data)
{
    for (char byte : data) {
        _data.push_back(byte);
    }
    _header.sh_size = sizeof(char) * _data.size();
}

void CustomSection::add_data(instruction_t instruction)
{
    for (size_t i = 0; i < sizeof(instruction_t); i++) {
        _data.push_back((char) ((instruction >> (8 * i)) & 0xFF));
    }
    _header.sh_size = sizeof(char) * _data.size();
}

void CustomSection::overwrite_data(void* data, size_t data_size, Elf32_Off offset)
{
    if (data == nullptr) {
        THROW_EXCEPTION("Data to overwrite is null.");
    } else if (offset + data_size > _data.size()) {
        THROW_EXCEPTION("Data to overwrite is out of bounds.");
    }

    char* c_data = (char*) data;
    for (size_t i = 0; i < data_size; i++) {
        _data[offset + i] = c_data[i];
    }
}

void CustomSection::replace_data(const std::vector<char>& data)
{
    _data = data;
    _header.sh_size = sizeof(char) * _data.size();
}

const char* CustomSection::get_data(Elf32_Off offset)
{
    return &_data[offset];
}

size_t CustomSection::get_size() const
{
    return _data.size();
}

RelocationTable& CustomSection::get_relocation_table()
{
    if (_relocation_table == nullptr) {
        _relocation_table = &_elf32_file.new_relocation_table(*this);
    }
    return *_relocation_table;
}

const std::string& CustomSection::get_name() const
{
    return _elf32_file.get_string_table().get_string(_header.sh_name);
}

bool CustomSection::has_relocation_table()
{
    return _relocation_table != nullptr;
}

void CustomSection::set_relocation_table(RelocationTable& relocation_table)
{
    _relocation_table = &relocation_table;
}

void CustomSection::print(std::ostream& ostream) const
{
    // clang-format off
    ostream << std::endl << "Content of section " << get_name() << ":";
    for (uint32_t i = 0; i < _data.size(); i++) {
        if (i % 16 == 0) {
            ostream << std::endl 
                    << std::right << std::hex << std::setw(8) << std::setfill('0') << i << ": ";
        }
        // TODO: change unsigned char cast
        ostream << std::hex << std::setw(2) << std::setfill('0') << (uint32_t) (unsigned char) _data[i] << " ";
    }
    ostream << std::left << std::dec << std::endl;
    // clang-format on
}

void CustomSection::write(std::ofstream& file)
{
    _header.sh_size = _data.size();

    if (file.tellp() % _header.sh_addralign != 0) {
        file.write("\0", _header.sh_addralign - (file.tellp() % _header.sh_addralign));
    }

    _header.sh_offset = file.tellp();
    file.write(_data.data(), _data.size());

    if (_relocation_table != nullptr) {
        _relocation_table->write(file);
    }
}