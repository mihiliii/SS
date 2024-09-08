#include "../inc/StringTable.hpp"

#include <iomanip>

#include "StringTable.hpp"

StringTable::StringTable(SectionHeaderTable* _sht, Elf32_Shdr* _section_header, std::vector<char> _str_table_data)
    : Section(_sht, _section_header) {
    for (int i = 0; i < (int) _str_table_data.size(); i++) {
        uint32_t offset = i;
        std::string str;
        while (_str_table_data[i] != '\0') {
            str += _str_table_data[i];
            i++;
        }
        string_table.insert(std::make_pair(offset, str));
    }
    _sht->setStringTable(this);
}

StringTable::StringTable(SectionHeaderTable* _sht) : Section(_sht) {
    section_header->sh_name = addString(".strtab");
    section_header->sh_type = SHT_STRTAB;
    section_header->sh_entsize = 0;
    section_header->sh_addralign = 1;
    _sht->setStringTable(this);
}

// Adds string to the string table and returns its offset
Elf32_Off StringTable::addString(std::string _string) {
    Elf32_Off offset = section_header->sh_size;
    string_table.insert(std::make_pair(offset, _string));
    section_header->sh_size += _string.size() + 1;
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
    section_header->sh_offset = _file->tellp();

    for (auto& pair : string_table) {
        _file->write(pair.second.c_str(), pair.second.size() + 1);
    }
}
