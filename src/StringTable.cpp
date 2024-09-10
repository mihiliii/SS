#include "../inc/StringTable.hpp"

#include <iomanip>

#include "../inc/Elf32_File.hpp"

StringTable::StringTable(Elf32_File* _elf32_file, std::vector<char> _str_table_data)
    : elf32_file(_elf32_file), string_table() {
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

StringTable::StringTable(Elf32_File* _elf32_file) : elf32_file(_elf32_file), string_table() {}

// Adds string to the string table and returns its offset
Elf32_Off StringTable::add(std::string _string) {
    Elf32_Off offset = string_table.rbegin()->first + string_table.rbegin()->second.size() + 1;
    string_table.insert(std::make_pair(offset, _string));
    elf32_file->getElf32Header().e_strsize += _string.size() + 1;
    return offset;
}

// Returns string from the string table by its offset
std::string StringTable::get(Elf32_Off _offset) {
    if (string_table.find(_offset) == string_table.end())
        return "";
    else
        return string_table[_offset];
}

// Returns offset of the string in the string table if it exists, otherwise returns -1
Elf32_Off StringTable::get(std::string _string) {
    for (auto& pair : string_table) {
        if (pair.second == _string)
            return pair.first;
    }
    return -1;
}

void StringTable::write(std::ofstream* _file) {
    elf32_file->getElf32Header().e_stroff = _file->tellp();
    elf32_file->getElf32Header().e_strsize = string_table.rbegin()->first + string_table.rbegin()->second.size() + 1;

    for (auto& pair : string_table) {
        _file->write(pair.second.c_str(), pair.second.size() + 1);
    }

}
