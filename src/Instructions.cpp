#include "../inc/Instructions.hpp"

#include <cstdint>
#include <string>
#include <unordered_map>

#include "../inc/Assembler.hpp"

std::unordered_map<std::string, uint8_t> OC_MOD = {
    {"halt", 0x00}, {"int", 0x01}, {"add", 0x50}, {"sub", 0x51}, {"mul", 0x52}, {"div", 0x53}
};

void Instructions::iHALT() {}

void Instructions::arithmetic(std::string _instruction, uint8_t _gprS, uint8_t _gprD) {
    uint8_t instruction_format[4] = {
        0,
        (uint8_t) (_gprS << 4),
        (uint8_t) (_gprD << 4 | _gprD),
        OC_MOD[_instruction]
    };

    Assembler::current_section->appendContent((char*) instruction_format, sizeof(uint8_t) * 4);
    Assembler::current_section->printContent();
    Assembler::increaseLocationCounter(4);
}