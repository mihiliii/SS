#pragma once

#include <vector>
#include <string>
#include "CPU.hpp"
#include "../Elf32File.hpp"

typedef std::vector<char> Memory;

class Emulator {
public:

    void loadProgram(const std::string& _path);


private:

    Elf32File* elf32_file;
    
    Memory memory;
    CPU cpu;

};