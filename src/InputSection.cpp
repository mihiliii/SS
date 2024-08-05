#include "../inc/InputSection.hpp"

#include <iomanip>
#include <iostream>

InputSection::InputSection(const std::string& _name) : Section() {
    SectionHeaderStringTable::getInstance().setSectionName(this, _name);
    this->name = _name;
}

void InputSection::appendContent(const char* _content, size_t _size) {
    for (size_t i = 0; i < _size; i++) {
        this->content.push_back(_content[i]);
    }
    this->section_header.sh_size += sizeof(char) * _size;
}

void InputSection::appendContent(char _content, size_t _number_of_repetitions) {
    for (size_t i = 0; i < _number_of_repetitions; i++) {
        this->content.push_back(_content);
    }
    this->section_header.sh_size += sizeof(char) * _number_of_repetitions;
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
