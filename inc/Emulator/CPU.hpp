#pragma once

#include <cstdint>

typedef uint32_t Register;

#define PC      GPR[15]
#define SP      GPR[14]
#define ZERO    GPR[0]

#define STATUS  CSR[0]
#define HANDLER CSR[1]
#define CAUSE   CSR[2]

class CPU {
public:

    // clang-format off
    Register& getGPR(uint8_t _index) { return GPR[_index]; };
    Register& getCSR(uint8_t _index) { return CSR[_index]; };
    Register& getPC()                { return PC; };
    Register& getSP()                { return SP; };
    Register& getZERO()              { return ZERO; };
    // clang-format on

    void execute();

    void reset();

private:

    Register GPR[16];
    Register CSR[3];
};