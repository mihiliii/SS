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

    Elf32_Shdr* getSectionHeader() const { return section_header; }

    ~Section() = default;

protected:

    Section();

    std::vector<T> content;
    Elf32_Shdr* section_header;

};


#endif

