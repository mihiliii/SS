#include "../inc/StringTable.hpp"

#include <iomanip>
#include "StringTable.hpp"

void StringTable::addString(std::string _string, Elf32_Off* _offset) {
    *_offset = section_header.sh_size;
    const char* c = _string.c_str();
    do {
        this->content.push_back(*c);
    } while (*c++ != '\0');
    section_header.sh_size += _string.size() + 1;
}

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

StringTable::StringTable() : Section() {
    name = ".strtab";
    addString(name, &section_header.sh_name);
    section_header.sh_entsize = 0;
}

// void StringTable::printContentHex() const {
    // std::cout << "Content of section " << name << ":\n";
    // for (uint32_t location_counter = 0; location_counter < content.size(); location_counter++) {
        // if (location_counter % 16 == 0) {
            // std::cout << std::hex << std::setw(8) << std::setfill('0') << location_counter << ": ";
        // }
        // std::cout << std::hex << std::setw(2) << std::setfill('0')
                  // << (unsigned int) (unsigned char) content[location_counter] << " ";
        // if ((location_counter + 1) % 16 == 0) {
            // std::cout << std::dec << "\n";
        // }
    // }
    // std::cout << std::dec << "\n";
// }

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