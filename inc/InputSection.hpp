#pragma once

#include <string>

#include "Section.hpp"
#include "StringTable.hpp"
#include "SectionHeaderTable.hpp"

class InputSection : public Section {
public:

    InputSection(const std::string& _name);

    void appendContent(void* _content, size_t _size);

    Elf32_Off getLocationCounter() const { return content.size(); };

    void printContent() const override;

    void write(std::ofstream* _file) override;

    ~InputSection() = default;

private:

    std::vector<char> content;
};
