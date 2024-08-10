#pragma once

#include <string>

#include "Section.hpp"
#include "StringTable.hpp"
#include "SectionHeaderTable.hpp"

class InputSection : public Section {
public:

    InputSection(const std::string& _name);

    void appendContent(const char* _content, size_t _size);

    void printContent() const;

    void setSize(size_t _length);

    void increaseLocationCounter(Elf32_Off _increment);

    ~InputSection() = default;

private:

    Elf32_Off location_counter;
    std::vector<char> content;
};
