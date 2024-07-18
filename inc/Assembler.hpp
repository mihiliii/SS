#ifndef Assembler_hpp_
#define Assembler_hpp_

#include <iostream>
#include <map>

class Assembler {
public:

    void readInstruction() {}

    static void increaseLocationCounter();

    static int startAssembler();

    Assembler() = delete;

    ~Assembler() = delete;

private:

    static int location_counter;
};

#endif