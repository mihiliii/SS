#include "../../inc/Emulator/Emulator.hpp"

#include <iomanip>
#include <stdexcept>

#include "../../inc/CustomSection.hpp"
#include "../../inc/Elf32File.hpp"

Emulator::Emulator(Elf32File& _program) : program(_program), memory(0x100000000, 0), cpu(memory) {
    if (_program.elf32Header().e_type != ET_EXEC) {
        throw std::runtime_error("Error: Invalid ELF file type.");
    }

    // Memory initialization
    for (auto& section : program.customSectionMap()) {
        for (size_t i = 0; i < section.second.content().size(); i++) {
            memory[section.second.header().sh_addr + i] = section.second.content()[i];
        }
    }
}

void Emulator::start() {
    cpu.run();
}

void Emulator::printEndState() {
    std::cout << "-----------------------------------------------------------------" << std::endl;
    std::cout << "Emulated processor executed halt instruction" << std::endl;
    for (int column = 0; column < 4; column++) {
        for (int row = 0; row < 4; row++) {
            if (column * 4 + row < 10) {
                std::cout << " ";
            }
            std::cout << "r" << std::dec << column * 4 + row << "=0x" << std::setw(8) << std::setfill('0') << std::hex
                      << cpu.GPR[column * 4 + row] << "   ";
        }
        std::cout << std::endl;
    }
}
