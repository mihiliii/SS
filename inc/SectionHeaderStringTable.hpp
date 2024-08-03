#pragma once

#include "Elf32.hpp"
#include "Section.hpp"

class SectionHeaderStringTable : Section<char> {
public:

    static SectionHeaderStringTable& getInstance();

    template <typename T>
    void setSectionName(Section<T>* _section, const std::string& _content);

    template <typename T>
    std::string getSectionName(Section<T>* _section) const;

    void printContent() const;

    SectionHeaderStringTable(const SectionHeaderStringTable&) = delete;
    SectionHeaderStringTable& operator=(const SectionHeaderStringTable&) = delete;

    ~SectionHeaderStringTable() = default;

private:

    SectionHeaderStringTable();
};

template <typename T>
void SectionHeaderStringTable::setSectionName(Section<T>* _section, const std::string& _content) {
    _section->getSectionHeader().sh_name = this->section_header.sh_size;
    const char* c = _content.c_str();
    do {
        this->content.push_back(*c);
    } while (*c++ != '\0');
    this->section_header.sh_size += _content.size() + 1;
}

template <typename T>
std::string SectionHeaderStringTable::getSectionName(Section<T>* _section) const {
    std::string name;
    for (size_t i = _section->getSectionHeader().sh_name; this->content[i] != '\0'; i++) {
        name.push_back(this->content[i]);
    }
    return name;
};
