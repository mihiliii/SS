#include "../inc/StringTable.hpp"

#include <iomanip>

void StringTable::addString(std::string _string, Elf32_Off* _offset) {
    *_offset = section_header.sh_size;
    const char* c = _string.c_str();
    do {
        this->content.push_back(*c);
    } while (*c++ != '\0');
    section_header.sh_size += _string.size() + 1;
}

std::string StringTable::getString(Elf32_Off _offset) { return std::string((const char*) &content[_offset]); }

StringTable& StringTable::getInstance() {
    static StringTable instance;
    return instance;
}

void StringTable::write(std::ofstream* _file) {
    section_header.sh_size = content.size();
    section_header.sh_offset = _file->tellp();


    _file->write(content.data(), content.size());
    // allignment to 4 bytes
    // for (int i = 0; i < 4 - (content.size() % 4); i++) {
    // _file->put('\0');
    // }
}

StringTable::StringTable() : Section(std::string(".strtab")) {
    addString(name, &section_header.sh_name);
    section_header.sh_entsize = 0;
}

void StringTable::printContent() const {
    std::cout << "       ***  STRING TABLE  ***       " << std::endl;
    int i = 0;
    for (char c : content) {
        std::cout << c;
        if (i++ % 16 == 0) {
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
}
