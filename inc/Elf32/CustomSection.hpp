#pragma once

#include "Assembler/InstructionFormat.hpp"

#include "Section.hpp"

#include <string>
#include <vector>

class RelocationTable;
typedef uint32_t instruction_format_t;

class CustomSection : public Section {
public:

    CustomSection(Elf32File& elf32_file, const std::string& name);

    CustomSection(Elf32File& elf32_file, const std::string& name, Elf32_Shdr section_header,
                  const std::vector<Elf32_Byte>& data);

    CustomSection(const CustomSection&) = delete;

    CustomSection(CustomSection&&);

    CustomSection& operator=(const CustomSection&) = delete;

    CustomSection& operator=(CustomSection&&);

    ~CustomSection() = default;

    void append_data(void* content, size_t content_size);

    void append_data(instruction_format_t content);

    void append_data(Elf32_Byte content);

    void append_data(const std::vector<Elf32_Byte>& content);

    void overwrite_data(void* content, size_t content_size, Elf32_Off offset);

    void overwrite_data(std::vector<Elf32_Byte> content, Elf32_Off offset);

    void replace_data(std::vector<Elf32_Byte> content);

    const std::vector<Elf32_Byte>& get_data() const;

    instruction_format get_instruction(size_t index) const;

    size_t get_size() const;

    RelocationTable& get_rela_table();

    bool has_rela_table();

    void set_rela_table(RelocationTable& relocation_table);

    void write(std::ostream& ostream) override;

    void print(std::ostream& ostream) const override;

private:

    std::vector<Elf32_Byte> _section_content;
    RelocationTable* _rela_table;
};
