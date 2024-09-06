#include "../inc/StringTable.hpp"

#include <iomanip>

StringTable& StringTable::getInstance() {
    static StringTable instance;
    return instance;
}

// Adds string to the string table and returns its offset
Elf32_Off StringTable::addString(std::string _string) {
    Elf32_Off offset = section_header.sh_size;
    string_table.insert(std::make_pair(offset, _string));
    section_header.sh_size += _string.size() + 1;
    return offset;
}

// Returns string from the string table by its offset
std::string StringTable::getString(Elf32_Off _offset) {
    if (string_table.find(_offset) == string_table.end())
        return "";
    else
        return string_table[_offset];
}

// Returns offset of the string in the string table if it exists, otherwise returns -1
Elf32_Off StringTable::findString(std::string _string) {
    for (auto& pair : string_table) {
        if (pair.second == _string)
            return pair.first;
    }
    return -1;
}

void StringTable::write(std::ofstream* _file) {
    section_header.sh_offset = _file->tellp();

    for (auto& pair : string_table) {
        _file->write(pair.second.c_str(), pair.second.size() + 1);
    }
}

StringTable::StringTable() : Section() {
    section_header.sh_name = addString(".strtab");
    section_header.sh_type = SHT_STRTAB;
    section_header.sh_entsize = 0;
    section_header.sh_addralign = 1;
}
