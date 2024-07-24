#include "../inc/Instructions.hpp"

#include <string>
#include <unordered_map>

#include "../inc/Assembler.hpp"

std::unordered_map<std::string, uint8_t> OC_MOD = {
    {"halt", 0x00}, {"int", 0x01}, {"add", 0x50}, {"sub", 0x51}, {"mul", 0x52}, {"div", 0x53}
};

// std::unordered_map<std::string, uint8_t> REGA_REGB = {
// {"halt", 0x00}
// };

// std::unordered_map<std::string, uint8_t> REGC_DISP = {
// {"halt", 0x00}
// };

// std::unordered_map<std::string, uint8_t> DISP_DISP = {
// {"halt", 0x00}
// };

std::unordered_map<std::string, std::function<void()>> Instructions::instruction_map = {
    {"halt", Instructions::iHALT}
};

void Instructions::iHALT() {
    return;
}
