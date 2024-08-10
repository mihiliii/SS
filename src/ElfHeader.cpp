#include "../inc/ElfHeader.hpp"
#include "../inc/Assembler.hpp"
#include <fstream>

ElfHeader& ElfHeader::getInstance() {
    static ElfHeader instance;
    return instance;
}

void ElfHeader::write(std::ofstream* _file) {
    _file->write(reinterpret_cast<char*>(&elf_header), sizeof(Elf32_Ehdr));
    Assembler::increaseLocationCounter(sizeof(Elf32_Ehdr));
}
