#include "../inc/StringTable.hpp"

#include <iomanip>

Elf32_Off StringTable::addString(std::string _string) {
    Elf32_Off offset = section_header.sh_size;
    const char* c = _string.c_str();
    do {
        content.push_back(*c);
    } while (*c++ != '\0');
    section_header.sh_size += _string.size() + 1;
    return offset;
}


std::string StringTable::getString(Elf32_Off _offset) { return std::string((const char*) &content[_offset]); }

void StringTable::write(std::ofstream* _file) {
    section_header.sh_size = content.size();
    section_header.sh_offset = _file->tellp();

    _file->write(content.data(), content.size());

    // allignment to 4 bytes
    // for (int i = 0; i < 4 - (content.size() % 4); i++) {
    // _file->put('\0');
    // }
}

StringTable::StringTable() : Section() {
    section_header.sh_name = addString(".strtab");
    section_header.sh_type = SHT_STRTAB;
    section_header.sh_entsize = 0;
}
