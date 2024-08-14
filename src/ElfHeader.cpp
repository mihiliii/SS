#include "../inc/ElfHeader.hpp"

#include <fstream>

#include "../inc/Assembler.hpp"
#include "ElfHeader.hpp"

ElfHeader& ElfHeader::getInstance() {
    static ElfHeader instance;
    return instance;
}

void ElfHeader::setField(ElfHeaderField _field, uint32_t _value) {
    switch (_field) {
        case SH_OFFSET:
            elf_header.e_shoff = _value;
            break;
        case SH_ENTRYSIZE:
            elf_header.e_shentsize = _value;
            break;
        case SH_NUM:
            elf_header.e_shnum = _value;
            break;
    }
}

void ElfHeader::write(std::ofstream* _file) {
    _file->write(reinterpret_cast<char*>(&elf_header), sizeof(Elf32_Ehdr));
}

ElfHeader::ElfHeader() { elf_header.e_shentsize = sizeof(Elf32_Shdr); }
