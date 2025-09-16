#pragma once

#include <list>
#include <map>
#include <string>

#include "../Elf32/Elf32File.hpp"

class Linker {
public:

    Linker(std::list<Elf32File*> input_files, std::map<std::string, Elf32_Addr> place_addresses);

    int start_linker(const std::string& output_file_name);

private:

    void map_custom_sections();

    void map_symbols();

    void map_symbol_table(Elf32File& input_file, std::list<Elf32_Sym*>& duplicate_symbols);

    void map_relocations();

    void map_relocation_table(Elf32File& input_file);

    Elf32File _output_file;
    std::list<Elf32File*> _input_files;
    std::map<std::string, Elf32_Addr> _place_addresses;
    std::map<std::pair<Elf32File*, CustomSection*>, Elf32_Off> _data_section_offsets;
};
