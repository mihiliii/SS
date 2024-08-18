#pragma once

#include <string>

#include "Section.hpp"

class CustomSection : public Section {
public:

    CustomSection(const std::string& _name);

    void appendContent(void* _content, size_t _content_size);

    void overwriteContent(void* _content, size_t _content_size, Elf32_Off _offset);

    Elf32_Off getLocationCounter() const { return content.size(); };

    void print() const;

    void write(std::ofstream* _file) override;

    static std::map<std::string, CustomSection*> getAllSections() { return all_sections; };

    ~CustomSection() = default;

private:

    std::vector<char> content;
    static std::map<std::string, CustomSection*> all_sections;
};
