#pragma once

#include <map>
#include <string>
#include <vector>

#include "../Elf32/Elf32.hpp"

class Elf32File;

struct Place_arg {
    std::string section;
    Elf32_Addr address;
};

class Linker {
public:

    static void addArgument(Place_arg place_arg);

    static int startLinking(const std::string& _output_file, std::vector<std::string> _input_files);

    static void map(Elf32File& _in_elf32_file);

private:

    static Elf32File out_elf32_file;
    static std::map<std::string, Elf32_Addr> place_arguments;
};
