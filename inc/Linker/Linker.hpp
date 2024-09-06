#pragma once

#include <string>
#include <vector>
#include <fstream>

#include "../Elf32.hpp"

struct Place_arg {
    std::string section;
    Elf32_Addr address;
};

class Linker {
public:

    Linker();

    ~Linker();

    void addArgument(Place_arg place_arg);

    int startLinking(const char* _output_file, std::vector<const char*> _input_files);

    void mapping(std::vector<const char*> _input_files);

private:

    std::vector<Place_arg> place_arguments;
    std::ofstream output_file;

        
};