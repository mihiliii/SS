#pragma once

#include <string>
#include <vector>
#include <fstream>

#include "../Elf32.hpp"

class SectionHeaderTable;

struct Place_arg {
    std::string section;
    Elf32_Addr address;
};

class Linker {
public:

    static void addArgument(Place_arg place_arg);

    static int startLinking(const char* _output_file, std::vector<const char*> _input_files);

    static void mapping(std::vector<const char*> _input_files);

private:

    static Elf32File* output_file;
    static std::vector<Place_arg> place_arguments;
        
};