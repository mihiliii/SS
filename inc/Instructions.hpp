#ifndef Instructions_hpp_
#define Instructions_hpp_

#include <functional>
#include <unordered_map>

#define Instruction uint32_t

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

private:

    static std::unordered_map<std::string, std::function<void()>> instruction_map;
};

#endif
