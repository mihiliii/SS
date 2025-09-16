#pragma once

#include "CPU.hpp"
#include "Elf32/Elf32File.hpp"
#include <vector>

typedef std::vector<char> Memory;

std::string gpr_to_s(REG reg);
std::string csr_to_s(REG reg);

void print_function(OC oc, MOD mod, REG regA, REG regB, REG regC, int disp);

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
