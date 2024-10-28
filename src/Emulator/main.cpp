#include "../../inc/Elf32File.hpp"
#include "../../inc/Emulator/Emulator.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Error, invalid arguments." << std::endl;
        return -1;
    }

    Elf32File elf32_file = Elf32File(std::string(argv[1]));

    Emulator emulator = Emulator(elf32_file);
    emulator.start();

    return 0;
}