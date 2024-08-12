#include <iostream>

#include "../inc/Assembler.hpp"
#include "../inc/Section.hpp"
#include "../inc/SymbolTable.hpp"

int main() {
    std::cout << "\nProgram start: \n\n";

    if (Assembler::startAssembler() == -1) {
        std::cout << "Assembler failed to start. \n";
        return -1;
    }

    if (Assembler::writeToFile() == -1) {
        std::cout << "Assembler failed to write to file. \n";
        return -1;
    } else {
        Assembler::readElfFile();
    }

    std::cout << "\nProgram end. \n";
    return 0;
}
