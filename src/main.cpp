#include "../inc/SymbolTable.hpp"
#include "../inc/SectionContent.hpp"
#include "../inc/Assembler.hpp"
#include <iostream>

int main() {
    std::cout << "\n\nProgram start: \n\n";
    Assembler::startAssembler();
    std::cout << "\n\nProgram end: \n\n";
    return 0;
}