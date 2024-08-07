#include "../inc/StringTable.hpp"

#include <iomanip>

Elf32_Off StringTable::addString(std::string _string) {
    Elf32_Off offset = section_header.sh_size;
    const char* c = _string.c_str();
    do {
        this->content.push_back(*c);
    } while (*c++ != '\0');
    section_header.sh_size += _string.size() + 1;
    return offset;
}

StringTable& StringTable::getInstance() {
    static StringTable instance;
    return instance;
}

StringTable::StringTable() : Section() {
    name = ".strtab";
    section_header.sh_name = addString(name);
}

void StringTable::printContentHex() const {
    std::cout << "Content of section " << name << ":\n";
    for (uint32_t location_counter = 0; location_counter < content.size(); location_counter++) {
        if (location_counter % 16 == 0) {
            std::cout << std::hex << std::setw(8) << std::setfill('0') << location_counter << ": ";
        }
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << (unsigned int) (unsigned char) content[location_counter] << " ";
        if ((location_counter + 1) % 16 == 0) {
            std::cout << std::dec << "\n";
        }
    }
    std::cout << std::dec << "\n";
}

void StringTable::printContent() const {
    for (char c : content) {
        std::cout << c;
    }
    std::cout << std::endl;
}