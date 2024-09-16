#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <map>

#include "../Elf32.hpp"

class Elf32File;

struct Place_arg {
    std::string section;
    Elf32_Addr address;
};

class Linker {
public:

    static void addArgument(Place_arg place_arg);

    static int startLinking(const char* _output_file, std::vector<const char*> _input_files);

    static void mapping(std::vector<const char*> _input_files);

    static void positioning();

    static void resolutioning();

private:

    static Elf32File* elf32_out;
    static std::map<std::string, Elf32_Addr> place_arguments;
        
};