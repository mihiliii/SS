#pragma once

#include <string>
#include <vector>

#include "Section.hpp"

class RelocationTable;
typedef uint32_t instruction_format_t;

class CustomSection : public Section {
public:

    CustomSection(Elf32File& elf32_file, const std::string& name);
    CustomSection(Elf32File& elf32_file, const std::string& name, Elf32_Shdr section_header,
                  const std::vector<char>& data);

    CustomSection(const CustomSection&) = delete;
    CustomSection& operator=(const CustomSection&) = delete;
    CustomSection(CustomSection&&) = delete;

    void append(void* content, size_t content_size);
    void append(instruction_format_t content);

    void overwrite(void* content, size_t content_size, Elf32_Off offset);
    void replace(std::vector<char> content);

    char* content(Elf32_Off offset);
    std::vector<char>& content();

    size_t size() const;

    RelocationTable& relocationTable();
    bool hasRelocationTable();
    void setRelocationTable(RelocationTable* relocation_table);

    void print(std::ostream& ostream) const;
    void write(std::ofstream* file) override;

    ~CustomSection() = default;

private:

    std::vector<char> _section_content;
    RelocationTable* _relocation_table;
};
