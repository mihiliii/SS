#include "../inc/StringTable.hpp"

#include <map>

#include "../inc/Elf32File.hpp"
#include "../inc/misc/Exceptions.hpp"

StringTable::StringTable(Elf32File& _elf32_file)
    : Section(_elf32_file), _str_table(std::map<Elf32_Off, std::string>())
{
}

Elf32_Off StringTable::add_string(const std::string& string)
{
    const Elf32_Off offset = _header.sh_size;
    _header.sh_size += string.size() + 1;

    _str_table.emplace(offset, string);
    return offset;
}

const std::string& StringTable::get_string(Elf32_Off offset) const
{
    if (_str_table.find(offset) == _str_table.end()) {
        return _str_table.at(kNullStringOffset);
    } else {
        return _str_table.at(offset);
    }
}

Elf32_Off StringTable::get_string_offset(const std::string& string) const
{
    for (const auto& pair : _str_table) {
        if (pair.second == string) {  // pair.second is the string [std::string]
            return pair.first;        // pair.first is the offset [Elf32_Off]
        }
    }
    return kNullStringOffset;
}

void StringTable::replace_data(const std::vector<char>& str_table_data)
{
    _str_table.clear();
    for (Elf32_Word i = 0; i < str_table_data.size(); i++) {
        const Elf32_Off offset = i;
        std::string str = "";
        while (str_table_data[i] != '\0') {
            str += str_table_data[i++];
        }
        _str_table.emplace(offset, str);
    }
    _header.sh_size = str_table_data.size();
}

void StringTable::replace_data(const std::vector<std::string>& str_table_data)
{
    _header.sh_size = 0;
    _str_table.clear();
    for (const auto& str : str_table_data) {
        add_string(str);
    }
    // _header.sh_size is updated in add_string method
}

void StringTable::write(std::ofstream& file)
{
    _header.sh_offset = file.tellp();

    for (const auto& pair : _str_table) {
        file.write(pair.second.c_str(), pair.second.size() + 1);
    }
}