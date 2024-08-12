#include "../inc/Instructions.hpp"

#include <cstdint>
#include <string>
#include <unordered_map>

#include "../inc/Assembler.hpp"

typedef uint8_t instruction_format[4];

std::unordered_map<std::string, uint8_t> OC_MOD = {
    {"halt", 0x00},
    {"int", 0x01},
    {"add", 0x50},
    {"sub", 0x51},
    {"mul", 0x52},
    {"div", 0x53},
    {"not", 0x60},
    {"and", 0x61},
    {"or", 0x62},
    {"xor", 0x63},
    {"shl", 0x70},
    {"shr", 0x71}
};

void Instructions::iHALT() {
    instruction_format instruction = {0, 0, 0, OC_MOD["halt"]};

    Assembler::current_section->appendContent((char*) instruction, sizeof(instruction));
    // Assembler::current_section->increaseLocationCounter(sizeof(instruction));
}

void Instructions::arithmeticIns(std::string _instruction, uint8_t _gprS, uint8_t _gprD) {
    instruction_format instruction = {0, (uint8_t) (_gprS << 4), (uint8_t) (_gprD << 4 | _gprD), OC_MOD[_instruction]};

    Assembler::current_section->appendContent((char*) instruction, sizeof(instruction));
    // Assembler::current_section->increaseLocationCounter(sizeof(instruction));
}

void Instructions::logicIns(std::string _instruction, uint8_t _gprS, uint8_t _gprD) {
    instruction_format instruction = {0, (uint8_t) (_gprS << 4), (uint8_t) (_gprD << 4 | _gprD), OC_MOD[_instruction]};

    Assembler::current_section->appendContent((char*) instruction, sizeof(instruction));
    // Assembler::current_section->increaseLocationCounter(sizeof(instruction));
}

void Instructions::shiftIns(std::string _instruction, uint8_t _gprS, uint8_t _gprD) {
    instruction_format instruction = {0, (uint8_t) (_gprS << 4), (uint8_t) (_gprD << 4 | _gprD), OC_MOD[_instruction]};

    Assembler::current_section->appendContent((char*) instruction, sizeof(instruction));
    // Assembler::current_section->increaseLocationCounter(sizeof(instruction));
}
