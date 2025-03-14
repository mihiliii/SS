#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "Elf32.hpp"
#include "Section.hpp"

class RelocationTable;
typedef uint32_t instruction_t;

struct CustomSection : public Section {
public:

    friend class Elf32File;

    ~CustomSection() = default;

    const char* get_data(Elf32_Off offset);

    size_t get_size() const;

    const std::string& get_name() const;

    void add_data(void* data, size_t data_size);

    void add_data(const std::vector<char>& data);

    void add_data(instruction_t data);

    void overwrite_data(void* data, size_t data_size, Elf32_Off offset);

    RelocationTable& get_relocation_table();

    bool has_relocation_table();

    void set_relocation_table(RelocationTable& relocation_table);

    void print(std::ostream& ostream) const;

    void write(std::ofstream& file) override;

private:

    CustomSection(Elf32File& elf32_file, const std::string& name);

    CustomSection(Elf32File& elf32_file, const std::string& name, Elf32_Shdr section_header,
                  const std::vector<char>& data);

    CustomSection(const CustomSection&) = delete;

    CustomSection(CustomSection&&) = delete;

    CustomSection& operator=(const CustomSection&) = delete;

    CustomSection& operator=(CustomSection&&) = delete;

    void replace_data(const std::vector<char>& data);

    // TODO: Change to uint8_t instead of char
    std::vector<char> _data;
    RelocationTable* _relocation_table;
};
