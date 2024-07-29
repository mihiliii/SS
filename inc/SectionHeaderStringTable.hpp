#ifndef SectionHeaderStringTable_hpp_
#define SectionHeaderStringTable_hpp_

#include "Elf32.hpp"
#include "Section.hpp"

class SectionHeaderStringTable : Section<char> {
public:

    static SectionHeaderStringTable& getInstance();

    template <typename T>
    void setSectionName(Section<T>* _section, const std::string& _content) {
        _section->getSectionHeader()->sh_name = this->section_header->sh_size;
        const char* c = _content.c_str();
        do {
            this->content.push_back(*c);
        } while (*c++ != '\0');
        this->section_header->sh_size += _content.size() + 1;
    }

    void printContent() const;

    SectionHeaderStringTable(const SectionHeaderStringTable&) = delete;
    SectionHeaderStringTable& operator=(const SectionHeaderStringTable&) = delete;

    ~SectionHeaderStringTable() = default;

private:

    SectionHeaderStringTable();
};

#endif
