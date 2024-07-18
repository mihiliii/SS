#ifndef SectionContent_hpp_
#define SectionContent_hpp_

#include <iostream>
#include <string>

#include "Elf32.hpp"

class SectionContent {
public:

    SectionContent() : section_table_entry({}), content(new std::string()) {}

    ~SectionContent() { delete content; }

    void writeContent(const std::string& _content);

    void printContent();

    void printSectionTableEntry();

private:

    Elf32_Shdr   section_table_entry;
    std::string* content;
};

#endif