#pragma once

#include <string>
#include <vector>

#include "Section.hpp"

class LiteralTable;
class RelocationTable;
typedef uint32_t instruction_format;

class CustomSection : public Section {
public:

    CustomSection(Elf32File* _elf32_file, const std::string& _name);
    CustomSection(
        Elf32File* _elf32_file, const std::string& _name, Elf32_Shdr _section_header, const std::vector<char>& _data
    );

    void append(void* _content, size_t _content_size);
    void append(instruction_format _content);

    void overwrite(void* _content, size_t _content_size, Elf32_Off _offset);
    void replace(const std::vector<char>& _content);

    char* content(Elf32_Off _offset);
    std::vector<char>& content();

    size_t size() const;

    RelocationTable* relocationTable();
    void setRelocationTable(RelocationTable* _relocation_table);

    void print(std::ostream& _ostream) const;
    void write(std::ofstream* _file) override;

    ~CustomSection() = default;

private:

    std::vector<char> section_content;
    RelocationTable* relocation_table;
};
