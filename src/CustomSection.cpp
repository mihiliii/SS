#include "../inc/CustomSection.hpp"

#include <iomanip>
#include <iostream>

#include "../inc/Elf32File.hpp"
#include "../inc/LiteralTable.hpp"
#include "../inc/RelocationTable.hpp"
#include "../inc/StringTable.hpp"
#include "../inc/SymbolTable.hpp"
#include "CustomSection.hpp"

CustomSection::CustomSection(Elf32File* _elf32_file, const std::string& _name)
    : Section(_elf32_file), relocation_table(nullptr) {
    section_header.sh_type = SHT_CUSTOM;
    section_header.sh_entsize = 4;
    section_header.sh_addralign = 4;
    section_header.sh_name = elf32_file->stringTable().add(_name);
}

CustomSection::CustomSection(
    Elf32File* _elf32_file, const std::string& _name, Elf32_Shdr _section_header, const std::vector<char>& _data
)
    : Section(_elf32_file, _section_header), section_content(_data), relocation_table(nullptr) {
    section_header.sh_name = elf32_file->stringTable().add(_name);
}

void CustomSection::append(void* _content, size_t _content_size) {
    char* char_content = (char*) _content;
    for (size_t i = 0; i < _content_size; i++) {
        section_content.push_back(char_content[i]);
    }
    section_header.sh_size += sizeof(char) * _content_size;
}

void CustomSection::append(instruction_format_t _content) {
    uint32_t instruction = (uint32_t) _content;
    for (size_t i = 0; i < sizeof(_content); i++) {
        section_content.push_back((char) ((instruction >> (8 * i)) & 0xFF));
    }
    section_header.sh_size += sizeof(_content);
}

void CustomSection::overwrite(void* _content, size_t _content_size, Elf32_Off _offset) {
    char* char_content = (char*) _content;
    for (size_t i = 0; i < _content_size; i++) {
        section_content[_offset + i] = char_content[i];
    }
}

char* CustomSection::content(Elf32_Off _offset) {
    return &section_content[_offset];
}

std::vector<char>& CustomSection::content() {
    return section_content;
}

void CustomSection::replace(const std::vector<char>& _content) {
    section_content = _content;
    section_header.sh_size = section_content.size();
}

size_t CustomSection::size() const {
    return section_content.size();
}

RelocationTable* CustomSection::relocationTable() {
    if (relocation_table == nullptr) {
        relocation_table = new RelocationTable(elf32_file, this);
    }
    return relocation_table;
}

bool CustomSection::hasRelocationTable() const {
    return relocation_table != nullptr;
}

void CustomSection::setRelocationTable(RelocationTable* _relocation_table) {
    relocation_table = _relocation_table;
}

void CustomSection::print(std::ostream& _ostream) const {
    _ostream << std::endl << "Content of section " << name() << ":\n";
    for (uint32_t location_counter = 0; location_counter < section_content.size(); location_counter++) {
        if (location_counter % 16 == 0) {
            _ostream << std::hex << std::setw(8) << std::setfill('0') << location_counter << ": ";
        }
        _ostream << std::hex << std::setw(2) << std::setfill('0')
                 << (unsigned int) (unsigned char) section_content[location_counter] << " ";
        if ((location_counter + 1) % 16 == 0) {
            _ostream << std::dec << "\n";
        }
    }
    _ostream << std::dec << "\n";
}

void CustomSection::write(std::ofstream* _file) {
    section_header.sh_size = section_content.size();

    if (_file->tellp() % section_header.sh_addralign != 0) {
        _file->write("\0", section_header.sh_addralign - (_file->tellp() % section_header.sh_addralign));
    }

    section_header.sh_offset = _file->tellp();
    _file->write(section_content.data(), section_content.size());

    if (relocation_table != nullptr) {
        relocation_table->write(_file);
    }
}
