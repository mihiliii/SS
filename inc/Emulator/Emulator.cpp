#include "Emulator.hpp"

void Emulator::loadProgram(const std::string& _path) {
    elf32_file = new Elf32File(_path, ELF32FILE_WRITTEN);

}