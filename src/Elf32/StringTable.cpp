#include "../../inc/Elf32/StringTable.hpp"

#include "../../inc/Elf32/Elf32File.hpp"

StringTable::StringTable(Elf32File* _elf32_file) : Section(_elf32_file), string_table() {}

Elf32_Off StringTable::add(std::string _string) {
    Elf32_Off offset = header().sh_size;
    header().sh_size += _string.size() + 1;

    string_table.insert(std::make_pair(offset, _string));
    return offset;
}

std::string StringTable::get(Elf32_Off _offset) {
    if (string_table.find(_offset) == string_table.end()) {
        return "";
    } else {
        return string_table.at(_offset);
    }
}

Elf32_Off StringTable::get(std::string _string) {
    for (auto& pair : string_table) {
        if (pair.second == _string) {
            return pair.first;
        }
    }
    return 0;
}

void StringTable::replace(const std::vector<char>& _str_table_data) {
    string_table.clear();
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

void StringTable::write(std::ofstream* _file) {
    header().sh_offset = _file->tellp();

    for (auto& pair : string_table) {
        _file->write(pair.second.c_str(), pair.second.size() + 1);
    }
}
