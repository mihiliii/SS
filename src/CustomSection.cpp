#include "../inc/CustomSection.hpp"

#include <iomanip>
#include <iostream>

#include "../inc/Assembler.hpp"
#include "CustomSection.hpp"

std::map<std::string, CustomSection*> CustomSection::all_sections;

CustomSection::CustomSection(const std::string& _name) : Section(), literal_table(this) {
    section_header.sh_name = Assembler::string_table->addString(_name);
    section_header.sh_type = SHT_CUSTOM;
    section_header.sh_entsize = 4;
    all_sections[_name] = this;
}

void CustomSection::appendContent(void* _content, size_t _content_size) {
    char* char_content = (char*) _content;
    for (size_t i = 0; i < _content_size; i++) {
        content.push_back(char_content[i]);
    }
    section_header.sh_size += sizeof(char) * _content_size;
}

void CustomSection::appendContent(instruction_format _content) {
    uint32_t instruction = (uint32_t) _content;
    for (size_t i = 0; i < sizeof(_content); i++) {
        content.push_back((char) ((instruction >> (8 * i)) & 0xFF));
    }
    section_header.sh_size += sizeof(_content);
}

void CustomSection::overwriteContent(void* _content, size_t _content_size, Elf32_Off _offset) {
    char* char_content = (char*) _content;
    for (size_t i = 0; i < _content_size; i++) {
        content[_offset + i] = char_content[i];
    }
}

char* CustomSection::getContent(Elf32_Off _offset) { return &content[_offset]; }

void CustomSection::print() const {
    std::cout << "Content of section " << getName() << ":\n";
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

void CustomSection::write(std::ofstream* _file) {
    section_header.sh_size = content.size();
    section_header.sh_offset = _file->tellp();

    _file->write(content.data(), content.size());

    if (!literal_table.isEmpty()) {
        literal_table.writePool(_file);
    }
}
