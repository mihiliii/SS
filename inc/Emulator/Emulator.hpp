#pragma once

#include <vector>
#include <string>
#include "CPU.hpp"
#include "../Elf32File.hpp"

typedef std::vector<char> Memory;

class Emulator {
public:

    Emulator(Elf32File _program) : program(_program), memory(0x100000000, 0), cpu(memory) {
        cpu.reset();
    }

    void run();



private:

    Elf32File program;
    
    Memory memory;
    CPU cpu;

};