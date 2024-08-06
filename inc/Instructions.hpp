#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <cstdint>

#define OC_MASK     0x0000000F
#define MOD_MASK    0x000000F0
#define REGA_MASK   0x00000F00
#define REGB_MASK   0x0000F000
#define REGC_MASK   0x000F0000
#define DISP_MASK   0xFFF00000

class Instructions {
public:

    friend class Assembler;

    static void iHALT();

    static void iINT();

    static void arithmetic(std::string _instruction, uint8_t _gprS, uint8_t _gprD);

private:

    static std::unordered_map<std::string, std::function<void()>> instruction_map;
};
