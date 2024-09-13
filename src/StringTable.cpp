#include "../inc/StringTable.hpp"

#include <iomanip>

#include "../inc/Elf32File.hpp"

StringTable::StringTable(Elf32File* _elf32_file) : Section(_elf32_file), string_table() {
    string_table.insert(std::make_pair(0, ""));
    section_header.sh_name = add(".strtab");
    section_header.sh_type = SHT_STRTAB;
}

StringTable::StringTable(Elf32File* _elf32_file, Elf32_Shdr _section_header, std::vector<char> _str_table_data)
    : Section(_elf32_file, _section_header), string_table() {
    string_table.insert(std::make_pair(0, ""));
    for (int i = 0; i < (int) _str_table_data.size(); i++) {
        uint32_t offset = i;
        std::string str;
        while (_str_table_data[i] != '\0') {
            str += _str_table_data[i];
            i++;
        }
        string_table.insert(std::make_pair(offset, str));
    }
}

Elf32_Off StringTable::add(std::string _string) {
    Elf32_Off offset = string_table.rbegin()->first + string_table.rbegin()->second.size() + 1;
    string_table.insert(std::make_pair(offset, _string));
    section_header.sh_size += _string.size() + 1;
    return offset;
}

std::string StringTable::get(Elf32_Off _offset) {
    if (string_table.find(_offset) == string_table.end())
        return "";
    else
        return string_table[_offset];
}

Elf32_Off StringTable::get(std::string _string) {
    for (auto& pair : string_table) {
        if (pair.second == _string)
            return pair.first;
    }
    return -1;
}

void StringTable::write(std::ofstream* _file) {
    section_header.sh_offset = _file->tellp();
    section_header.sh_size = string_table.rbegin()->first + string_table.rbegin()->second.size() + 1;

    for (auto& pair : string_table) {
        _file->write(pair.second.c_str(), pair.second.size() + 1);
    }
}
