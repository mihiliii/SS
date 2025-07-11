#pragma once

#include <string>
#include <vector>

#include "Assembler.hpp"

struct Operand;
class CustomSection;
class Elf32File;
class ForwardReferenceTable;
class LiteralTable;

class Directives {
public:

    static void sectionDirective(const std::string& _section_name);

    static void skipDirective(int _bytes);

    static void wordDirective(std::vector<Operand>* _values);

    static void globalDirective(std::vector<Operand>* _symbols);

    static void externDirective(std::vector<Operand>* _symbols);

    static int defineLabel(std::string _label);

private:

    static CustomSection*& current_section;
    static Elf32File& elf32_file;
    static ForwardReferenceTable& forward_reference_table;
    static std::map<CustomSection*, LiteralTable>& literal_table_map;
};
