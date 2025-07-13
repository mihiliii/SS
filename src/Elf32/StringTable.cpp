#include "../../inc/Elf32/StringTable.hpp"

#include "../../inc/Elf32/Elf32File.hpp"

StringTable::StringTable(Elf32File& elf32_file) : Section(elf32_file), _string_table() {}

const Elf32_Off StringTable::add_string(const std::string& string) {
    Elf32_Off offset = _header.sh_size;
    _header.sh_size += string.size() + 1;

    _string_table.insert(std::make_pair(offset, string));
    return offset;
}

const std::string& StringTable::get_string(Elf32_Off offset) {
    if (_string_table.find(offset) == _string_table.end()) {
        return ""; // FIX: add empty string at index 0
    } else {
        return _string_table.at(offset);
    }
}

const Elf32_Off StringTable::get_offset(const std::string& string) {
    for (auto& pair : _string_table) {
        if (pair.second == string) {
            return pair.first;
        }
    }
    return 0;
}

void StringTable::replace(const std::vector<char>& str_table_data) {
    _string_table.clear();
    for (int i = 0; i < (int) str_table_data.size(); i++) {
        uint32_t offset = i;
        std::string str;
        while (str_table_data[i] != '\0') {
            str += str_table_data[i++];
        }
        _string_table.insert(std::make_pair(offset, str)); // NOTE: can be done better
    }
}

void StringTable::write(std::ofstream* _file) {
    _header.sh_offset = _file->tellp();

    for (const auto& pair : _string_table) {
        _file->write(pair.second.c_str(), pair.second.size() + 1);
    }
}
