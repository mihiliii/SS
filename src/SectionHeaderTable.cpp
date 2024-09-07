#include "../inc/SectionHeaderTable.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>

#include "../inc/Section.hpp"
#include "../inc/StringTable.hpp"
#include "../inc/SymbolTable.hpp"

SectionHeaderTable::SectionHeaderTable() : section_header_table(), section_header_table_index(0) {}

SectionHeaderTable::SectionHeaderTable(std::vector<Elf32_Shdr> _section_header_table)
    : section_header_table(), section_header_table_index(0) {
    for (auto& section : _section_header_table) {
        section_header_table.emplace(section_header_table_index, new Elf32_Shdr(section));
        section_header_table_index++;
    }
}

uint32_t SectionHeaderTable::add() {
    section_header_table.emplace(section_header_table_index, new Elf32_Shdr());
    return section_header_table_index++;
}

uint32_t SectionHeaderTable::add(Elf32_Shdr** _section_header_handle) {
    *_section_header_handle = new Elf32_Shdr();
    section_header_table.emplace(section_header_table_index, *_section_header_handle);
    return section_header_table_index++;
}

Elf32_Shdr* SectionHeaderTable::getSectionHeader(std::string _section_name) {
    for (auto& iterator : section_header_table) {
        Elf32_Shdr* section = iterator.second;
        if (str_table->getString(section->sh_name) == _section_name)
            return section;
    }
    return nullptr;
}

void SectionHeaderTable::write(std::ofstream* _file) {
    for (auto& iterator : section_header_table) {
        Elf32_Shdr* section = iterator.second;
        _file->write((char*) (section), sizeof(Elf32_Shdr));
    }
}

void SectionHeaderTable::print(std::ofstream& _file) {
    _file << std::endl << "Section Header Table:" << std::endl;
    _file << "  ";
    _file << std::left << std::setfill(' ');
    _file << std::setw(4) << "NUM";
    _file << std::setw(25) << "NAME";
    _file << std::setw(5) << "TYPE";
    _file << std::setw(9) << "ADDRESS";
    _file << std::setw(9) << "OFFSET";
    _file << std::setw(9) << "SIZE";
    _file << std::setw(9) << "FLAGS";
    _file << std::setw(5) << "LINK";
    _file << std::setw(5) << "INFO";
    _file << std::setw(6) << "ALIGN";
    _file << std::setw(9) << "ENTSIZE";
    _file << std::endl;
    uint32_t index = 0;
    for (auto& iterator : section_header_table) {
        Elf32_Shdr* section = iterator.second;
        _file << "  ";
        _file << std::setw(3) << std::right << std::dec << std::setfill(' ') << index << " ";
        _file << std::left;
        _file << std::setw(24) << str_table->getString(section->sh_name) << " ";
        _file << std::right << std::hex << std::setfill('0');
        _file << std::setw(4) << section->sh_type << " ";
        _file << std::setw(8) << section->sh_addr << " ";
        _file << std::setw(8) << section->sh_offset << " ";
        _file << std::setw(8) << section->sh_size << " ";
        _file << std::setw(8) << section->sh_flags << " ";
        _file << std::setw(4) << section->sh_link << " ";
        _file << std::setw(4) << section->sh_info << " ";
        _file << std::setw(5) << std::dec << std::setfill(' ') << std::left << section->sh_addralign << " ";
        _file << std::setw(8) << section->sh_entsize << std::endl;
        index++;
    }
}

SectionHeaderTable::~SectionHeaderTable() {
    for (auto& iterator : section_header_table) {
        delete iterator.second;
    }
}
