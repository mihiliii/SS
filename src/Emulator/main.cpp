#include "../../inc/Elf32File.hpp"
#include "../../inc/Emulator/Emulator.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Error, invalid arguments." << std::endl;
        return -1;
    }

    Emulator emulator = Emulator(Elf32File(argv[1]));
    emulator.start();

    return 0;
}