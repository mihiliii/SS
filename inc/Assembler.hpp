#ifndef Assembler_hpp_
#define Assembler_hpp_

#include <iostream>
#include "SectionContent.hpp"

class Assembler {
public:

    friend class Instructions;

    static void decodeInstruction(const std::string& string);

    static void decodeInstruction(char* string);

    static void printLocationCounter() { std::cout << location_counter; }

    static void increaseLocationCounter() { location_counter++; }

    static int startAssembler();
    
    static int writeToFile();

    Assembler() = delete;

    ~Assembler() = delete;

private:

    static int location_counter;
    static SectionContent* text_section;
};

#endif