#pragma once

#include <string>
#include <vector>

#include "Section.hpp"

class LiteralTable;
class RelocationTable;
typedef uint32_t instruction_format;

class CustomSection : public Section {
public:

    CustomSection(Elf32File* _elf32_file, std::string _name);
    CustomSection(Elf32File* _elf32_file, std::string _name, Elf32_Shdr _section_header, std::vector<char> _data);

    void append(void* _content, size_t _content_size);
    void append(instruction_format _content);

    void overwrite(void* _content, size_t _content_size, Elf32_Off _offset);

    char* getContent(Elf32_Off _offset);

    size_t size() const;

    LiteralTable* getLiteralTable();
    RelocationTable* getRelocationTable();

    void setRelocationTable(RelocationTable* _relocation_table);
    void setLiteralTable(LiteralTable* _literal_table);

    void print(std::ofstream& _file) const;
    void write(std::ofstream* _file) override;

    ~CustomSection() = default;

private:

    std::vector<char> content;
    LiteralTable* literal_table;
    RelocationTable* relocation_table;
};
