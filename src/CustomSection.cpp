#include "../inc/CustomSection.hpp"

#include <iomanip>
#include <iostream>

#include "../inc/LiteralTable.hpp"
#include "../inc/Elf32File.hpp"
#include "../inc/RelocationTable.hpp"
#include "../inc/StringTable.hpp"
#include "../inc/SymbolTable.hpp"

CustomSection::CustomSection(Elf32File* _elf32_file, std::string _name)
    : Section(_elf32_file), literal_table(new LiteralTable(_elf32_file, this)), relocation_table(nullptr) {
    section_header.sh_type = SHT_CUSTOM;
    section_header.sh_entsize = 4;
    section_header.sh_addralign = 4;
    section_header.sh_name = elf32_file->getStringTable().add(_name);
    elf32_file->getCustomSections().insert(std::pair<std::string, CustomSection*>(_name, this));
}

CustomSection::CustomSection(
    Elf32File* _elf32_file, std::string _name, Elf32_Shdr _section_header, std::vector<char> _data
)
    : Section(_elf32_file, _section_header),
      content(_data),
      literal_table(new LiteralTable(_elf32_file, this)),
      relocation_table(nullptr) {
    elf32_file->getCustomSections().insert(std::pair<std::string, CustomSection*>(_name, this));
}

void CustomSection::append(void* _content, size_t _content_size) {
    char* char_content = (char*) _content;
    for (size_t i = 0; i < _content_size; i++) {
        content.push_back(char_content[i]);
    }
    section_header.sh_size += sizeof(char) * _content_size;
}

void CustomSection::append(instruction_format _content) {
    uint32_t instruction = (uint32_t) _content;
    for (size_t i = 0; i < sizeof(_content); i++) {
        content.push_back((char) ((instruction >> (8 * i)) & 0xFF));
    }
    section_header.sh_size += sizeof(_content);
}

void CustomSection::overwrite(void* _content, size_t _content_size, Elf32_Off _offset) {
    char* char_content = (char*) _content;
    for (size_t i = 0; i < _content_size; i++) {
        content[_offset + i] = char_content[i];
    }
}

char* CustomSection::getContent(Elf32_Off _offset) {
    return &content[_offset];
}

std::vector<char>& CustomSection::getContent() {
    return content;
}

void CustomSection::replace(std::vector<char> _content) {
    content = _content;
    section_header.sh_size = content.size();
}

size_t CustomSection::size() const {
    return content.size();
}

LiteralTable* CustomSection::getLiteralTable() {
    if (literal_table == nullptr) {
        literal_table = new LiteralTable(elf32_file, this);
    }
    return literal_table;
}

RelocationTable* CustomSection::getRelocationTable() {
    if (relocation_table == nullptr) {
        relocation_table = new RelocationTable(elf32_file, this);
    }
    return relocation_table;
}

void CustomSection::setRelocationTable(RelocationTable* _relocation_table) {
    relocation_table = _relocation_table;
}

void CustomSection::setLiteralTable(LiteralTable* _literal_table) {
    literal_table = _literal_table;
}

void CustomSection::print(std::ostream& _ostream) const {
    _ostream << std::endl << "Content of section " << name() << ":\n";
    for (uint32_t location_counter = 0; location_counter < content.size(); location_counter++) {
        if (location_counter % 16 == 0) {
            _ostream << std::hex << std::setw(8) << std::setfill('0') << location_counter << ": ";
        }
        _ostream << std::hex << std::setw(2) << std::setfill('0')
              << (unsigned int) (unsigned char) content[location_counter] << " ";
        if ((location_counter + 1) % 16 == 0) {
            _ostream << std::dec << "\n";
        }
    }
    _ostream << std::dec << "\n";
}

void CustomSection::write(std::ofstream* _file) {
    section_header.sh_size = content.size();

    if (_file->tellp() % section_header.sh_addralign != 0) {
        _file->write("\0", section_header.sh_addralign - (_file->tellp() % section_header.sh_addralign));
    }

    section_header.sh_offset = _file->tellp();
    _file->write(content.data(), content.size());

    if (literal_table != nullptr) {
        literal_table->write(_file);
    }

    if (relocation_table != nullptr) {
        relocation_table->write(_file);
    }
}
