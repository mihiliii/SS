#pragma once

#include <fstream>
#include <iostream>
#include <vector>

#include "Elf32.hpp"

class ParseElf32 {
public:

    ParseElf32();

    // Constructor that opens the file
    ParseElf32(std::ifstream* _file, const char* _file_name);

    // Constructor that assumes the file is already open
    ParseElf32(std::ifstream* _file);

    // Opens the file and sets it to read from
    void file(std::ifstream* _file, const char* _file_name);

    // Sets the file to read from
    void setFile(std::ifstream* _file);

    Elf32_Ehdr readElfHeader();

    std::vector<Elf32_Shdr> readSectionHeaderTable();

private:

    std::ifstream* elf32_file;
};