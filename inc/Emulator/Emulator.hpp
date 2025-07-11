#pragma once

#include "../Elf32/Elf32File.hpp"
#include "CPU.hpp"
#include <vector>

typedef std::vector<char> Memory;

class Emulator {
public:

    Emulator(Elf32File& _program);

    void start();

    void printEndState();

private:

    Elf32File& program;

    Memory memory;
    CPU cpu;
};
