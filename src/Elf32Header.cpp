#include "../inc/Elf32Header.hpp"

#include <fstream>

Elf32Header::Elf32Header() {
    elf_header.e_shentsize = sizeof(Elf32_Shdr);
}

Elf32Header& Elf32Header::getInstance() {
    static Elf32Header instance;
    return instance;
}

void Elf32Header::setField(Elf32_Ehdr_Field _field, uint32_t _value) {
    switch (_field) {
        case Elf32_Ehdr_Field::e_type:
            elf_header.e_type = _value;
            break;
        case Elf32_Ehdr_Field::e_entry:
            elf_header.e_entry = _value;
            break;
        case Elf32_Ehdr_Field::e_phoff:
            elf_header.e_phoff = _value;
            break;
        case Elf32_Ehdr_Field::e_shoff:
            elf_header.e_shoff = _value;
            break;
        case Elf32_Ehdr_Field::e_phentsize:
            elf_header.e_phentsize = _value;
            break;
        case Elf32_Ehdr_Field::e_phnum:
            elf_header.e_phnum = _value;
            break;
        case Elf32_Ehdr_Field::e_shentsize:
            elf_header.e_shentsize = _value;
            break;
        case Elf32_Ehdr_Field::e_shnum:
            elf_header.e_shnum = _value;
            break;
        case Elf32_Ehdr_Field::e_shstrndx:
            elf_header.e_shstrndx = _value;
            break;
        default:
            break;
    }
}

void Elf32Header::write(std::ofstream* _file) {
    _file->write(reinterpret_cast<char*>(&elf_header), sizeof(Elf32_Ehdr));
}

void Elf32Header::print(std::ofstream& _file) {
    _file << std::endl << "Elf Header:" << std::endl;
    _file << "  Identification: ";
    for (int i = 0; i < EI_NIDENT; i++) {
        _file << (elf_header.e_ident[i]) << " ";
    }
    _file << std::hex << std::endl;
    switch (elf_header.e_type) {
        case ET_NONE:
            _file << "  Type: No file type" << std::endl;
            break;
        case ET_REL:
            _file << "  Type: Relocatable file" << std::endl;
            break;
        case ET_EXEC:
            _file << "  Type: Executable file" << std::endl;
            break;
        case ET_DYN:
            _file << "  Type: Shared object file" << std::endl;
            break;
        default:
            break;
    }
    _file << "  Entry point address: 0x" << elf_header.e_entry << std::endl;
    _file << "  Section header offset: 0x" << elf_header.e_shoff << std::endl;
    _file << std::dec;
    _file << "  Section header entry size: " << elf_header.e_shentsize << " (bytes)" << std::endl;
    _file << "  Number of section headers: " << elf_header.e_shnum << std::endl;
    _file << "  Section header string table index: " << elf_header.e_shstrndx << std::endl;
    _file << std::hex;
    _file << "  Page header offset: 0x" << elf_header.e_phoff << std::endl;
    _file << std::dec;
    _file << "  Page header entry size: " << elf_header.e_phentsize << " (bytes)" << std::endl;
    _file << "  Number of page headers: " << elf_header.e_phnum << std::endl;
}
