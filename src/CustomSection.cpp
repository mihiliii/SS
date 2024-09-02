#include "../inc/CustomSection.hpp"

#include <iomanip>
#include <iostream>

#include "../inc/Assembler.hpp"
#include "CustomSection.hpp"

std::map<std::string, CustomSection*> CustomSection::all_sections;

CustomSection::CustomSection(const std::string& _name) : Section(_name), literal_table(this), relocation_table(nullptr) {
    section_header.sh_type = SHT_CUSTOM;
    section_header.sh_entsize = 4;
    section_header.sh_addralign = 4;
    all_sections[_name] = this;
    Assembler::symbol_table->addSymbol(
        _name, 0, true, section_header_table_index, ELF32_ST_INFO(STB_LOCAL, STT_SECTION)
    );
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

char* CustomSection::getContent(Elf32_Off _offset) {
    return &content[_offset];
}

RelocationTable& CustomSection::getRelocationTable() {
    if (relocation_table == nullptr) {
        relocation_table = new RelocationTable(this);
    }
    return *relocation_table;
}

void CustomSection::print(std::ofstream& _file) const {

    _file << std::endl << "Content of section " << getName() << ":\n";
    for (uint32_t location_counter = 0; location_counter < content.size(); location_counter++) {
        if (location_counter % 16 == 0) {
            _file << std::hex << std::setw(8) << std::setfill('0') << location_counter << ": ";
        }
        _file << std::hex << std::setw(2) << std::setfill('0')
                  << (unsigned int) (unsigned char) content[location_counter] << " ";
        if ((location_counter + 1) % 16 == 0) {
            _file << std::dec << "\n";
        }
    }
    _file << std::dec << "\n";
}

void CustomSection::write(std::ofstream* _file) {
    section_header.sh_size = content.size();

    if (_file->tellp() % section_header.sh_addralign != 0) {
        _file->write("\0", section_header.sh_addralign - (_file->tellp() % section_header.sh_addralign));
    }

    section_header.sh_offset = _file->tellp();
    _file->write(content.data(), content.size());

    if (!literal_table.isEmpty()) {
        literal_table.writePool(_file);
    }

    if (relocation_table != nullptr) {
        relocation_table->write(_file);
    }
}
