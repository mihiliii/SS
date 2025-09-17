#include "Linker/Linker.hpp"

#include "Elf32/Elf32File.hpp"

#include <algorithm>
#include <iostream>

Linker::Linker(std::list<Elf32File*> input_files, std::map<std::string, Elf32_Addr> place_addresses)
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

    return 0;
}

void Linker::map_symbol_table(Elf32File& input_file, std::list<Elf32_Sym*>& duplicate_symbols)
{
    for (Elf32_Sym in_symbol : input_file.symbol_table.get_symbol_table()) {

        const std::string& symbol_name = input_file.string_table.get_string(in_symbol.st_name);
        Elf32_Sym* out_symbol = _output_file.symbol_table.find_symbol(symbol_name);

        if (in_symbol.st_shndx != SHN_ABS) {
            Elf32_Shdr& sh = *input_file.section_header_table.at(in_symbol.st_shndx);
            const std::string& section_name = input_file.string_table.get_string(sh.sh_name);

            CustomSection& section = _output_file.custom_section_map.at(section_name);

            in_symbol.st_shndx = section.get_header_index();

            if (ELF32_ST_TYPE(in_symbol.st_info) == STT_SECTION) {
                in_symbol.st_value = section.get_header().sh_addr;
            }
            else {
                auto it_offset = _data_section_offsets.find({&input_file, &section});
                if (it_offset != _data_section_offsets.end()) {
                    in_symbol.st_value += it_offset->second;
                }
                in_symbol.st_value += section.get_header().sh_addr;
            }
        }

        if (out_symbol == nullptr) {
            _output_file.symbol_table.add_symbol(symbol_name, in_symbol);
            continue;
        }

        if (out_symbol->st_defined == true && in_symbol.st_defined == true &&
            ELF32_ST_TYPE(in_symbol.st_info) != STT_SECTION) {
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
        map_symbol_table(*input_file, duplicate_symbols);
    }

    for (auto& symbol : _output_file.symbol_table.get_symbol_table()) {
        if (symbol.st_defined == false) {
            undefined_symbols.push_back(&symbol);
        }
    }

    for (auto& symbol : duplicate_symbols) {
        std::cout << "Linker Error: Duplicate symbol definition: "
                  << _output_file.string_table.get_string(symbol->st_name) << std::endl;
    }

    for (auto& symbol : undefined_symbols) {
        std::cout << "Linker Error: Undefined symbol: "
                  << _output_file.string_table.get_string(symbol->st_name) << std::endl;
    }

    if (duplicate_symbols.size() > 0 || undefined_symbols.size() > 0) {
        throw std::runtime_error("Linker Error: Cannot continue linking due to errors.");
    }

    // sort symbol table by section type and then by st_shndx and then by value

    auto& sym_tab = _output_file.symbol_table.get_symbol_table();
    std::stable_sort(sym_tab.begin(), sym_tab.end(), [](const Elf32_Sym& a, const Elf32_Sym& b) {
        if (ELF32_ST_TYPE(a.st_info) != ELF32_ST_TYPE(b.st_info)) {
            return ELF32_ST_TYPE(a.st_info) > ELF32_ST_TYPE(b.st_info);
        }
        if (a.st_shndx != b.st_shndx) {
            return a.st_shndx < b.st_shndx;
        }
        return a.st_value < b.st_value;
    });
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
            Elf32_Word out_symbol_index = _output_file.symbol_table.get_symbol_index(sym_entry);

            rela_entry.r_info = ELF32_R_INFO(ELF32_R_TYPE(rela_entry.r_info), out_symbol_index);

            auto it_offset = _data_section_offsets.find({&input_file, &out_section});
            if (it_offset != _data_section_offsets.end()) {
                rela_entry.r_offset += it_offset->second;
            }

            out_rela_table_data.push_back(rela_entry);
        }

        // creates new relocation table if not exists, otherwise appends data to existing one
        auto it = _output_file.rela_table_map.find(rela_name);
        if (it == _output_file.rela_table_map.end()) {
            Elf32_Shdr rela_header = in_rela_table.get_header();
            rela_header.sh_link = ELF32_ST_INFO(ELF32_ST_TYPE(rela_header.sh_link),
                                                ELF32_ST_BIND(out_section.get_header_index()));

            _output_file.new_relocation_table(rela_name, out_section, rela_header,
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
        map_relocation_table(*input_file);
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
        for (const auto& [section_name, section] : input_file->custom_section_map) {

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

                _data_section_offsets.emplace(std::make_pair(input_file, &output_section),
                                              output_section.get_size());

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
