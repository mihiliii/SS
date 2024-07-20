#include "../inc/SymbolTable.hpp"
#include "../inc/SectionContent.hpp"
#include "../inc/Assembler.hpp"
#include <iostream>

int main() {
    std::cout << "\nProgram start: \n\n";

    if (Assembler::startAssembler() == -1) {
        std::cout << "Assembler failed to start. \n";
        return -1;
    }

    if (Assembler::writeToFile() == -1) {
        std::cout << "Assembler failed to write to file. \n";
        return -1;
    }

    std::cout << "\nProgram end. \n";
    return 0;
}