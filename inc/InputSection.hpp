#pragma once

#include <string>

#include "Section.hpp"
#include "SectionHeaderStringTable.hpp"
#include "SectionHeaderTable.hpp"

class InputSection : public Section {
public:

    InputSection(const std::string& _name);

    void appendContent(const char* _content, size_t _size);

    void appendContent(char _content, size_t _number_of_repetitions);

    void printContent() const;

    ~InputSection() = default;

private:

    std::vector<char> content;
};
