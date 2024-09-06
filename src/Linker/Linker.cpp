#include "../../inc/Linker/Linker.hpp"

#include <iostream>

#include "../../inc/ParseElf32.hpp"
#include "../../inc/SectionHeaderTable.hpp"
#include "../../inc/CustomSection.hpp"
#include "../../inc/StringTable.hpp"
#include "../../inc/SymbolTable.hpp"

Linker::Linker() {}

Linker::~Linker() {}

void Linker::addArgument(Place_arg place_arg) {
    place_arguments.push_back(place_arg);
}

int Linker::startLinking(const char* _output_file, std::vector<const char*> _input_files) {
    std::cout << "Linking started" << std::endl;

    return 0;
}

void Linker::mapping(std::vector<const char*> _input_files) {
    SectionHeaderTable& new_sht = SectionHeaderTable::getInstance();
    StringTable& new_str_table = StringTable::getInstance();
    SymbolTable& new_sym_table = SymbolTable::getInstance();

    ParseElf32 elf_file_reader = ParseElf32();

    for (auto input_file : _input_files) {
        std::ifstream file;
        elf_file_reader.file(&file, input_file);
        Elf32_Ehdr elf_header = elf_file_reader.readElfHeader();
        std::vector<Elf32_Shdr> old_sht_vector = elf_file_reader.readSectionHeaderTable();

        for (auto old_SH : old_sht_vector) {
            // Check if the section header is a string table, if yes then do not add it to the new section header table
            if (old_SH.sh_type == SHT_STRTAB || old_SH.sh_type == SHT_SYMTAB) {
                continue;
            }

            // If the section header is not in the new section header table, add it
            if (new_sht.getSectionHeader(old_SH.sh_name) == nullptr) {
                CustomSection* new_section = new CustomSection(old_SH.sh_name);
                Elf32_Shdr* new_SH = new Elf32_Shdr(old_SH);
                new_sht.insert(&old_SH);
            }
            // Else, add the size of the section to the existing section
            else {
                new_sht.getSectionHeader(old_SH.sh_name)->sh_size += old_SH.sh_size;
            }
        }
    }

    for (auto place_arg : place_arguments) {
        new_sht.getSectionHeader(place_arg.section)->sh_addr = place_arg.address;
    }

    Elf32_Addr location_counter = 0;

    for (auto section_header : new_sht.getSectionHeaderTable()) {
        if (section_header.second->sh_addr == 0) {
            section_header.second->sh_offset = location_counter;
            location_counter += section_header.second->sh_size;
        }
    }

    // Dont forget to delete all the pointers
}
