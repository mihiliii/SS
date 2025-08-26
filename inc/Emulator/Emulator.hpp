#pragma once

#include "../Elf32/Elf32File.hpp"
#include "CPU.hpp"
#include <vector>

typedef std::vector<char> Memory;

class Emulator {
public:

    Emulator(Elf32File& program_file);

    void start_emulator();

    void print_end_state();

private:

    Elf32File& _program;

    Memory _memory;
    CPU _cpu;
};
