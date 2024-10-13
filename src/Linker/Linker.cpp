#include "../../inc/Linker/Linker.hpp"

#include <iostream>

#include "../../inc/CustomSection.hpp"
#include "../../inc/Elf32File.hpp"
#include "../../inc/StringTable.hpp"
#include "../../inc/SymbolTable.hpp"

std::map<std::string, Elf32_Addr> Linker::place_arguments;
Elf32File Linker::out_elf32_file = Elf32File();

void Linker::addArgument(Place_arg place_arg) {
    place_arguments.insert(std::pair<std::string, Elf32_Addr>(place_arg.section, place_arg.address));
}

int Linker::startLinking(const std::string& _output_file, std::vector<std::string> _input_files) {
    std::cout << "Linking started" << std::endl;

    // mapping phase

    for (auto input_file : _input_files) {
        Elf32File in_elf32_file = Elf32File(input_file);
        map(in_elf32_file);
    }

    Elf32_Addr out_current_place_address = 0;
    for (const auto& place_arguments_iterator : place_arguments) {
        const std::string& section_name = place_arguments_iterator.first;
        const Elf32_Addr& place_address = place_arguments_iterator.second;
        CustomSection& out_section = out_elf32_file.customSectionMap().find(section_name)->second;

        if (place_address == 0) {
            out_section.header().sh_addr = out_current_place_address;
            out_current_place_address += out_section.size();
        } else {
            out_section.header().sh_addr = place_arguments_iterator.second;
        }
    }

    out_elf32_file.write(_output_file, ELF32FILE_EXEC);
    Elf32File::readElf(_output_file);
    return 0;
}

void Linker::map(Elf32File& in_elf32_file) {
    // Adds sections to out_elf32_file. If the section does not exist, create a new one and add it
    // to the output file and add it to place_arguments map so linker can change its address later.
    // If section exists in out_elf32_file then append content only.

    for (auto& in_cs_map_iterator : in_elf32_file.customSectionMap()) {
        const std::string& in_section_name = in_cs_map_iterator.first;
        CustomSection& in_section = in_cs_map_iterator.second;

        auto out_cs_map_iterator = out_elf32_file.customSectionMap().find(in_section_name);
        if (out_cs_map_iterator == out_elf32_file.customSectionMap().end()) {
            out_elf32_file.newCustomSection(in_section_name, in_section.header(), in_section.content());

            if (place_arguments.find(in_section_name) == place_arguments.end()) {
                place_arguments.insert(std::pair<std::string, Elf32_Addr>(in_section_name, 0));
            }
        } else {
            CustomSection& out_section = out_cs_map_iterator->second;

            out_section.append((char*) in_section.content().data(), in_section.size());
        }
    }

    // Resolve symbol table.

    for (auto symbol : in_elf32_file.symbolTable().symbolTable()) {
        const std::string& symbol_name = in_elf32_file.stringTable().get(symbol.st_name);

        if (out_elf32_file.symbolTable().get(symbol_name) == nullptr) {
            out_elf32_file.symbolTable().add(symbol_name, symbol);
        }
    }

    // Resolve relocation tables.

    for (auto& in_rela_table_iterator : in_elf32_file.relocationTableMap()) {
        RelocationTable& in_rela_table = in_rela_table_iterator.second;
        CustomSection& in_section = *in_rela_table_iterator.first;

        std::vector<Elf32_Rela> out_rela_table_content;
        for (Elf32_Rela relocation : in_rela_table.relocationTable()) {
            Elf32_Sym* sym_entry = in_elf32_file.symbolTable().get(ELF32_R_SYM(relocation.r_info));
            std::string symbol_name = in_elf32_file.stringTable().get(sym_entry->st_name);

            relocation.r_info =
                ELF32_R_INFO(ELF32_R_TYPE(relocation.r_info), out_elf32_file.symbolTable().getIndex(symbol_name));

            out_rela_table_content.push_back(relocation);
        }

        auto out_section_iterator = out_elf32_file.customSectionMap().find(in_section.name());
        if (out_section_iterator == out_elf32_file.customSectionMap().end()) {
            std::cerr << "Error: Could not find section in out_elf32_file in Linker::map" << std::endl;
            return;
        }

        auto out_rela_table_iterator = out_elf32_file.relocationTableMap().find(&out_section_iterator->second);
        if (out_rela_table_iterator == out_elf32_file.relocationTableMap().end()) {
            out_elf32_file.newRelocationTable(&out_section_iterator->second, in_rela_table.header(),
                                              out_rela_table_content);

        } else {
            RelocationTable& out_rela_table = out_rela_table_iterator->second;
            out_rela_table.add(out_rela_table_content);
        }
    }
}

// void Linker::positioning() {
//     for (auto& symbol : out_elf32_file->symbolTable().getContent()) {
//         if (symbol->st_shndx != SHN_ABS && symbol->st_shndx != SHN_UNDEF)
//             symbol->st_value += out_elf32_file->sectionHeaderTable()[symbol->st_shndx]->sh_addr;
//     }
// }

// void Linker::resolutioning() {
//     for (auto& relocation_tables : out_elf32_file->relocationTableMap()) {
//         RelocationTable* relocation_table = relocation_tables.second;

//         for (auto& relocation_entry : relocation_table->getContent()) {
//             Elf32_Sym* symbol =
//                 out_elf32_file->symbolTable().get(ELF32_R_SYM(relocation_entry.r_info));
//             if (symbol->st_shndx != SHN_ABS && symbol->st_shndx != SHN_UNDEF)
//                 symbol->st_value +=
//                 out_elf32_file->sectionHeaderTable()[symbol->st_shndx]->sh_addr;
//         }
//     }
// }
