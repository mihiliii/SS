#pragma once

#include <string>
#include <vector>

#include "LiteralTable.hpp"
#include "Section.hpp"

typedef uint32_t instruction_format;

class CustomSection : public Section {
public:

    CustomSection(const std::string& _name);

    void appendContent(void* _content, size_t _content_size);

    void appendContent(instruction_format _content);

    void overwriteContent(void* _content, size_t _content_size, Elf32_Off _offset);

    char* getContent(Elf32_Off _offset);

    Elf32_Off getLocationCounter() const { return content.size(); };

    LiteralTable& getLiteralTable() { return literal_table; };

    void print() const;

    void write(std::ofstream* _file) override;

    static std::map<std::string, CustomSection*> getSectionsMap() { return all_sections; };

    ~CustomSection() = default;

private:

    std::vector<char> content;
    LiteralTable literal_table;

    static std::map<std::string, CustomSection*> all_sections;
};
