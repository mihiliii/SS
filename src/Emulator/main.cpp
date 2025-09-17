#include "Emulator/Emulator.hpp"

#include "Elf32/Elf32File.hpp"

Emulator* emulator = nullptr;

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Emulator Error: Incorrect argument usage.\n"
                  << "Correct usage: " << std::string(argv[0]) << " <input_file>";
        return -1;
    }

    try {

        Elf32File elf32_file = Elf32File(argv[1]);

        emulator = new Emulator(elf32_file);
        emulator->start_emulator();

        emulator->print_end_state();
    }
    catch (const std::exception& e) {
        std::cerr << "Emulator Error: " << e.what() << std::endl;

        delete emulator;

        return -1;
    }

    delete emulator;

    return 0;
}
