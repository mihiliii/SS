#ifndef Section_hpp_
#define Section_hpp_

#include <iostream>
#include <string>
#include <vector>

#include "Elf32.hpp"

class SectionHeaderStringTable;

template <typename T>
class Section {
public:

    Section(const std::string& _name);

    ~Section() {}

    void appendContent(T* _content, size_t _size);

    void setName(const std::string& _name);

    void setType(Elf32_Word _type);

    void printContent();

    std::vector<T>* getContent() const { return &content; }

protected:

    std::vector<T> content;
    Elf32_Shdr section_header;

    static SectionHeaderStringTable* section_header_string_table;
};

#endif

