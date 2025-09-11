#include "../../inc/Linker/Linker.hpp"
#include "../../inc/Elf32/CustomSection.hpp"
#include "../../inc/Elf32/Elf32File.hpp"
#include "../../inc/Elf32/StringTable.hpp"

#include <iostream>

Linker::Linker(const std::list<Elf32File>& input_files,
               std::map<std::string, Elf32_Addr> place_addresses)
    : _output_file(Elf32File()),
      _input_files(input_files),
      _place_addresses(place_addresses)
{
}

int Linker::start_linker(const std::string& output_file_name)
{
    map_custom_sections();
    map_symbols();
    map_relocations();

    _output_file.write_bin(output_file_name.substr(0, output_file_name.rfind('.')) + ".o", ET_EXEC);
    _output_file.write_hex(output_file_name);
    _output_file.read_elf(output_file_name.substr(0, output_file_name.rfind('.')) + ".o");

    return 0;
}

void Linker::map_symbol_table(Elf32File& input_file, std::list<Elf32_Sym*>& duplicate_symbols)
{
    for (auto in_symbol : input_file.symbol_table.get_symbol_table()) {

        const std::string& symbol_name = input_file.string_table.get_string(in_symbol.st_name);
        Elf32_Sym* out_symbol = _output_file.symbol_table.find_symbol(symbol_name);

        if (in_symbol.st_shndx != SHN_ABS) {
            Elf32_Shdr& section_header = *input_file.section_header_table.at(in_symbol.st_shndx);
            const std::string& section_name =
                input_file.string_table.get_string(section_header.sh_name);

            const CustomSection& section = _output_file.custom_section_map.at(section_name);

            in_symbol.st_shndx = section.get_header_index();
            in_symbol.st_value += section.get_header().sh_addr;
        }

        if (out_symbol == nullptr) {
            _output_file.symbol_table.add_symbol(symbol_name, in_symbol);

            continue;
        }

        if (out_symbol->st_defined == true && in_symbol.st_defined == true) {
            duplicate_symbols.push_back(out_symbol);
        }
        else if (out_symbol->st_defined == false && in_symbol.st_defined == true) {
            _output_file.symbol_table.set_symbol(*out_symbol, symbol_name, in_symbol);
        }
    }
}

void Linker::map_symbols()
{
    std::list<Elf32_Sym*> duplicate_symbols;
    std::list<Elf32_Sym*> undefined_symbols;

    for (auto& input_file : _input_files) {
        map_symbol_table(input_file, duplicate_symbols);
    }

    for (auto& symbol : _output_file.symbol_table.get_symbol_table()) {
        if (symbol.st_defined == false) {
            undefined_symbols.push_back(&symbol);
        }
    }

    for (auto& symbol : duplicate_symbols) {
        std::cout << "Error: Duplicate symbol definition: "
                  << _output_file.string_table.get_string(symbol->st_name) << std::endl;
    }

    for (auto& symbol : undefined_symbols) {
        std::cout << "Error: Undefined symbol: "
                  << _output_file.string_table.get_string(symbol->st_name) << std::endl;
    }

    if (duplicate_symbols.size() > 0 || undefined_symbols.size() > 0) {
        exit(-1);
    }
}

void Linker::map_relocation_table(Elf32File& input_file)
{
    for (auto& [rela_name, in_rela_table] : input_file.rela_table_map) {

        std::string section_name = in_rela_table.get_linked_section().get_name();
        CustomSection& out_section = _output_file.custom_section_map.at(section_name);
        std::vector<Elf32_Rela> out_rela_table_data;

        for (Elf32_Rela rela_entry : in_rela_table.get_relocation_table()) {

            Elf32_Sym sym_entry =
                input_file.symbol_table.get_symbol(ELF32_R_SYM(rela_entry.r_info));
            const std::string& symbol_name = input_file.string_table.get_string(sym_entry.st_name);
            Elf32_Word out_symbol_index = _output_file.symbol_table.get_symbol_index(symbol_name);

            rela_entry.r_info = ELF32_R_INFO(ELF32_R_TYPE(rela_entry.r_info), out_symbol_index);
            rela_entry.r_offset += out_section.get_header().sh_addr;

            out_rela_table_data.push_back(rela_entry);
        }

        // creates new relocation table if not exists, otherwise appends data to existing one
        auto it = _output_file.rela_table_map.find(rela_name);
        if (it == _output_file.rela_table_map.end()) {
            _output_file.new_relocation_table(rela_name, out_section, in_rela_table.get_header(),
                                              out_rela_table_data);
        }
        else {
            RelocationTable& out_rela_table = it->second;
            out_rela_table.add_entry(out_rela_table_data);
        }
    }
}

void Linker::map_relocations()
{
    for (auto& input_file : _input_files) {
        map_relocation_table(input_file);
    }

    for (auto& [rela_name, rela_table] : _output_file.rela_table_map) {
        CustomSection& section = rela_table.get_linked_section();

        for (auto& rela_entry : rela_table.get_relocation_table()) {
            Elf32_Sym symbol = _output_file.symbol_table.get_symbol(ELF32_R_SYM(rela_entry.r_info));

            Elf32_Word value = symbol.st_value + rela_entry.r_addend;
            section.overwrite_data(&value, sizeof(value), rela_entry.r_offset);
        }
    }
}

void Linker::map_custom_sections()
{
    // Adds sections to out_elf32_file. If the section does not exist, create a new one and add
    // it to the output file and add it to place_arguments map so linker can change its address
    // later. If section exists in out_elf32_file then append content only.
    for (const auto& input_file : _input_files) {
        for (const auto& [section_name, section] : input_file.custom_section_map) {

            auto it = _output_file.custom_section_map.find(section_name);
            if (it == _output_file.custom_section_map.end()) {
                _output_file.new_custom_section(section_name, section.get_header(),
                                                section.get_data());

                if (_place_addresses.find(section_name) == _place_addresses.end()) {
                    _place_addresses.emplace(section_name, 0);
                }
            }
            else {
                CustomSection& output_section = it->second;

                output_section.append_data(section.get_data());
            }
        }
    }

    Elf32_Addr next_place_addr = 0;
    for (const auto& [section_name, place_addr] : _place_addresses) {

        CustomSection& output_section = _output_file.custom_section_map.at(section_name);
        Elf32_Shdr section_header = output_section.get_header();

        if (place_addr == 0) {
            section_header.sh_addr = next_place_addr;
            next_place_addr += output_section.get_size();
        }
        else {
            section_header.sh_addr = place_addr;
        }

        output_section.set_header(section_header);
    }
}
