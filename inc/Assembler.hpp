#ifndef Assembler_hpp_
#define Assembler_hpp_

#include <iostream>
#include <map>
#include "SectionContent.hpp"

class Assembler {
public:

    void readInstruction() {}

    static void printLocationCounter() { std::cout << location_counter; }

    static void increaseLocationCounter() { location_counter++; }

    static int startAssembler();

    Assembler() = delete;

    ~Assembler() = delete;

private:

    static int location_counter;
    static SectionContent* text_section;
};

#endif