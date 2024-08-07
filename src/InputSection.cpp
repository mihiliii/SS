#include "../inc/InputSection.hpp"

#include <iomanip>
#include <iostream>
#include "InputSection.hpp"


InputSection::InputSection(const std::string& _name) : Section() {
    this->name = _name;
    section_header.sh_name = StringTable::getInstance().addString(_name);
}

void InputSection::appendContent(const char* _content, size_t _content_size) {
    for (size_t i = 0; i < _content_size; i++) {
        this->content.push_back(_content[i]);
    }
    this->section_header.sh_size += sizeof(char) * _content_size;
}

void InputSection::printContent() const {
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

void InputSection::setSize(size_t _length) {
    this->section_header.sh_size = _length;
}
