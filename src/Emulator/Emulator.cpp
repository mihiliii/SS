#include "../../inc/Emulator/Emulator.hpp"

#include <iomanip>
#include <stdexcept>

#include "../../inc/Elf32/Elf32File.hpp"

Emulator::Emulator(Elf32File& program) : _program(program), _memory(0x100000000, 0), _cpu(_memory)
{
    if (_program.elf32_header.e_type != ET_EXEC) {
        throw std::runtime_error("Error: Invalid ELF file type.");
    }

    for (auto& it : program.custom_section_map) {
        CustomSection& section = it.second;
        Elf32_Addr addr = section.get_header().sh_addr;

        for (size_t i = 0; i < section.get_size(); i++) {
            _memory[addr + i] = section.get_data()[i];
        }
    }
}

void Emulator::start_emulator()
{
    _cpu.run();
}

void Emulator::print_end_state()
{
    const int max_column = 4;
    const int max_row = 4;
    std::ostream os_reg(std::cout.rdbuf());
    std::ostream os_value(std::cout.rdbuf());

    os_reg << std::dec << std::setfill(' ');
    os_value << std::hex << std::setfill('0');
    std::cout << "-----------------------------------------------------------------\n"
              << "Emulated processor executed halt instruction\n";

    for (int row = 0; row < max_row; row++) {
        for (int column = 0; column < max_column; column++) {
            const int i = row * max_column + column;

            if (i < 10) {
                std::cout << " ";
            }

            os_reg << "r" << row * 4 + column << "=0x";
            os_value << std::setw(8) << _cpu._gpr[i];
            if (column != max_column - 1) {
                os_value << "   ";
            }
        }
        std::cout << std::endl;
    }
}
