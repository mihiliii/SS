#include "../../inc/Linker/Linker.hpp"

#include <iostream>

#include "../../inc/CustomSection.hpp"
#include "../../inc/Elf32File.hpp"
#include "../../inc/StringTable.hpp"
#include "../../inc/SymbolTable.hpp"

std::map<std::string, Elf32_Addr> Linker::place_arguments;
Elf32File* Linker::elf32_out = nullptr;

void Linker::addArgument(Place_arg place_arg) {
    place_arguments.insert(std::pair<std::string, Elf32_Addr>(place_arg.section, place_arg.address));
}

int Linker::startLinking(const char* _output_file, std::vector<const char*> _input_files) {
    std::cout << "Linking started" << std::endl;

    elf32_out = new Elf32File();

    mapping(_input_files);
    return 0;
}

void Linker::mapping(std::vector<const char*> _input_files) {
    for (auto input_file : _input_files) {
        Elf32File* elf32_in = new Elf32File(input_file);

        for (auto& custom_section_in : elf32_in->getCustomSections()) {
            std::string section_name = custom_section_in.first;
            CustomSection* section = custom_section_in.second;
            CustomSection* new_section;

            auto custom_section_out = elf32_out->getCustomSections().find(section_name);
            if (custom_section_out == elf32_out->getCustomSections().end()) {
                new_section = new CustomSection(elf32_out, section_name, section->header(), section->getContent());
                if (place_arguments.find(section_name) == place_arguments.end())
                    place_arguments.insert(std::pair<std::string, Elf32_Addr>(section_name, 0));
            }
            else {
                new_section = custom_section_out->second;
                new_section->append((char*) section->getContent().data(), section->size());
            }
        }

        Elf32_Addr address = 0;
        for (auto& args : place_arguments) {
            CustomSection* section = elf32_out->getCustomSections().find(args.first)->second;
            if (args.second == 0) {
                section->header().sh_addr = address;
                address += section->size();
            }
            else {
                section->header().sh_addr = args.second;
            }
        }

        // Relocation tables
        for (auto& rela_table_in : elf32_in->getRelocationTables()) {
            CustomSection* section = rela_table_in.first;
            RelocationTable* relocation_t = rela_table_in.second;
            RelocationTable* new_relocation_t;

            auto rela_table_out = elf32_out->getRelocationTables().find(section);
            if (rela_table_out == elf32_out->getRelocationTables().end()) {
                new_relocation_t =
                    new RelocationTable(elf32_out, section, relocation_t->header(), relocation_t->getContent());
            }
            else {
                new_relocation_t = rela_table_out->second;
            }

            for (Elf32_Rela& relocation : relocation_t->getContent()) {
                new_relocation_t->add(relocation);
            }
        }

        // Symbol table
        for (auto& symbol : elf32_in->getSymbolTable().getContent())
            if (elf32_out->getSymbolTable().get(elf32_in->getStringTable().get(symbol->st_name)) == nullptr)
                elf32_out->getSymbolTable().add(elf32_in->getStringTable().get(symbol->st_name), *symbol);

        delete elf32_in;
    }
}

void Linker::positioning() {
    for (auto& symbol : elf32_out->getSymbolTable().getContent()) {
        if (symbol->st_shndx != SHN_ABS && symbol->st_shndx != SHN_UNDEF)
            symbol->st_value += elf32_out->getSectionHeaderTable()[symbol->st_shndx]->sh_addr;
    }
}

void Linker::resolutioning() {
    for (auto& relocation_tables : elf32_out->getRelocationTables()) {
        RelocationTable* relocation_table = relocation_tables.second;

        for (auto& relocation_entry : relocation_table->getContent()) {
            Elf32_Sym* symbol = elf32_out->getSymbolTable().get(ELF32_R_SYM(relocation_entry.r_info));
            if (symbol->st_shndx != SHN_ABS && symbol->st_shndx != SHN_UNDEF)
                symbol->st_value += elf32_out->getSectionHeaderTable()[symbol->st_shndx]->sh_addr;
        }
    }
}
