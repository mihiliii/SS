#pragma once

#include <vector>
#include <string>
#include "CPU.hpp"
#include "../Elf32File.hpp"

typedef std::vector<char> Memory;

class Emulator {
public:

    Emulator(Elf32File _program); 

    void start();


private:

    Elf32File program;
    
    Memory memory;
    CPU cpu;

};