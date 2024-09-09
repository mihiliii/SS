#include "../../inc/Linker/Linker.hpp"

#include <iostream>

#include "../../inc/CustomSection.hpp"
#include "../../inc/Linker/ParseElf32.hpp"
#include "../../inc/SectionHeaderTable.hpp"
#include "../../inc/StringTable.hpp"
#include "../../inc/SymbolTable.hpp"

void Linker::addArgument(Place_arg place_arg) {
    place_arguments.push_back(place_arg);
}

int Linker::startLinking(const char* _output_file, std::vector<const char*> _input_files) {
    std::cout << "Linking started" << std::endl;

    linker_sht = new SectionHeaderTable();
    linker_sht->setStringTable(new StringTable());
    linker_sht->setSymbolTable(new SymbolTable());

    this->mapping(_input_files);
    return 0;
}

void Linker::mapping(std::vector<const char*> _input_files) {
    for (auto input_file : _input_files) {
        ParseElf32 elf_file_reader = ParseElf32(std::string(input_file));

        std::ofstream output_txt_file("temp.txt", std::ios::out | std::ios::trunc);
        if (!output_txt_file.is_open()) {
            std::cerr << "Error opening file temp.txt" << std::endl;
            return;
        }

        // prints the elf file to the output file to check if there are any errors
        elf_file_reader.getElf32Header().print(output_txt_file);
        elf_file_reader.getSectionHeaderTable().print(output_txt_file);
        elf_file_reader.getSymbolTable().print(output_txt_file);

        for (auto section : CustomSection::getSectionsMap()) {
            section.second->print(output_txt_file);
            if (!section.second->getRelocationTable().isEmpty()) {
                section.second->getRelocationTable().print(output_txt_file);
            }
        }

        // adding the section headers to the linker's section header table 

        for (auto section_entry : elf_file_reader.getSectionHeaderTable().getSectionHeaderTable()) {
            linker_sht->addExisting(*section_entry.second);
        }

        linker_sht->print(output_txt_file);


        // for (auto old_SH : old_sht_vector) {
        //// Check if the section header is a string table, if yes then do not add it to the new section header table
        // if (old_SH.sh_type == SHT_STRTAB || old_SH.sh_type == SHT_SYMTAB) {
        // continue;
        //}

        //// If the section header is not in the new section header table, add it
        // if (new_sht.getSectionHeader(old_SH.sh_name) == nullptr) {
        // CustomSection* new_section = new CustomSection(old_SH.sh_name);
        // Elf32_Shdr* new_SH = new Elf32_Shdr(old_SH);
        // new_sht.insert(&old_SH);
        //}
        //// Else, add the size of the section to the existing section
        // else {
        // new_sht.getSectionHeader(old_SH.sh_name)->sh_size += old_SH.sh_size;
        //}
        //}
        //}

        // for (auto place_arg : place_arguments) {
        // new_sht.getSectionHeader(place_arg.section)->sh_addr = place_arg.address;
        //}

        // Elf32_Addr location_counter = 0;

        // for (auto section_header : new_sht.getSectionHeaderTable()) {
        // if (section_header.second->sh_addr == 0) {
        // section_header.second->sh_offset = location_counter;
        // location_counter += section_header.second->sh_size;
        //}
        //}
    }

    // Dont forget to delete all the pointers
}
