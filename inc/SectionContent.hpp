#ifndef SectionContent_hpp_
#define SectionContent_hpp_

#include <iostream>
#include <string>
#include <vector>

#include "Elf32.hpp"

class SectionContent {
public:

    SectionContent() : section_table_entry({}), content(new std::vector<uint8_t>()) {}

    ~SectionContent() { delete content; }

    void writeContent(uint8_t* content, size_t size);

    std::vector<uint8_t>* getContent() const { return content; }

    void printContent();

    void printSectionTableEntry();

private:

    Elf32_Shdr section_table_entry;
    std::vector<uint8_t>* content;
};

#endif
