#include "../../inc/Emulator/Emulator.hpp"

#include <stdexcept>

#include "../../inc/Elf32File.hpp"
#include "../../inc/CustomSection.hpp"

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
