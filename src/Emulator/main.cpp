#include "../../inc/Elf32/Elf32File.hpp"
#include "../../inc/Emulator/Emulator.hpp"

Emulator* emulator = nullptr;

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Error, invalid arguments." << std::endl;
        return -1;
    }

    Elf32File elf32_file = Elf32File(argv[1]);

    elf32_file.read_elf(argv[1]);

    emulator = new Emulator(elf32_file);
    emulator->start_emulator();

    emulator->print_end_state();

    delete emulator;

    return 0;
}
