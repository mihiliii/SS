#pragma once

#include <map>
#include <string>
#include <vector>

#include "../Elf32/Elf32File.hpp"

struct PlaceArg {
    std::string section;
    Elf32_Addr address;
};

class Linker {
public:

    void add_argument(PlaceArg place_arg);

    int start_linker(const std::string& output_file_name, std::vector<std::string> input_files);

    void map(Elf32File& input_file);

private:

    CustomSection& get_section(const std::string& section_name)
    {
        auto it = _output_file.custom_section_map.find(section_name);
        if (it == _output_file.custom_section_map.end()) {
            throw std::runtime_error("Section not found: " + section_name);
        }
        return it->second;
    }

    Elf32File _output_file;
    std::map<std::string, Elf32_Addr> _place_addresses;
};
