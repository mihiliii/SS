#include "../../inc/Linker/Linker.hpp"

#include <iostream>
#include <queue>

void Linker::add_argument(PlaceArg place_arg)
{
    _place_addresses.insert(
        std::pair<std::string, Elf32_Addr>(place_arg.section, place_arg.address));
}

int Linker::start_linker(const std::string& output_file_name, std::vector<std::string> input_files)
{

    // map phase

    for (auto input_file : input_files) {
        Elf32File in_elf32_file = Elf32File(input_file);
        map(in_elf32_file);
    }

    Elf32_Addr out_current_place_address = 0;
    for (const auto& it : _place_addresses) {
        const std::string& section_name = it.first;
        const Elf32_Addr& place_addr = it.second;

        auto output_it = _output_elf32_file.custom_section_map.find(section_name);
        if (output_it == _output_elf32_file.custom_section_map.end()) {
            std::cerr << "Error: no section named '" << section_name << "' in place arguments."
                      << std::endl;
            return -1;
        }

        CustomSection& out_section = output_it->second;
        Elf32_Shdr header = out_section.get_header();

        if (place_addr == 0) {
            header.sh_addr = out_current_place_address;
            out_current_place_address += out_section.get_size();
        }
        else {
            header.sh_addr = place_addr;
        }

        out_section.set_header(header);
    }

    // symbol resolve phase

    for (Elf32_Sym symbol : _output_elf32_file.symbol_table.get_symbol_table()) {

        // TODO: check if symbol definition check is needed
        if (symbol.st_defined == false) {
            std::cerr << "Error: Undefined symbol: "
                      << _output_elf32_file.string_table.get_string(symbol.st_name) << std::endl;
        }

        if (ELF32_ST_TYPE(symbol.st_info) == STT_SECTION) {
            const std::string& section_name =
                _output_elf32_file.string_table.get_string(symbol.st_name);

            const CustomSection& section =
                _output_elf32_file.custom_section_map.find(section_name)->second;

            // NOTE: what the fuck is this
            symbol.st_value = section.get_header().sh_addr;
        }
        else {
            // FIX: add bound checking for symbol.st_shndx
            Elf32_Shdr* temp = _output_elf32_file.section_header_table.at(symbol.st_shndx);

            const std::string& section_name =
                _output_elf32_file.string_table.get_string(temp->sh_name);
            CustomSection& section =
                _output_elf32_file.custom_section_map.find(section_name)->second;

            symbol.st_value += section.get_header().sh_addr;
        }
    }

    // relocation phase

    for (auto& it : _output_elf32_file.rela_table_map) {
        RelocationTable& rela_table = it.second;
        CustomSection& section = rela_table.get_linked_section();

        for (auto& rela_entry : rela_table.get_relocation_table()) {
            Elf32_Sym* symbol =
                _output_elf32_file.symbol_table.get_symbol(ELF32_R_SYM(rela_entry.r_info));

            if (symbol == nullptr) {
                std::cerr << "Error: Could not find symbol in relocation phase" << std::endl;
                return -1;
            }

            Elf32_Word value = symbol->st_value + rela_entry.r_addend;
            section.overwrite_data(&value, sizeof(value), rela_entry.r_offset);
        }
    }

    // NOTE: there should be only one section symbol in symbol table for each section

    const std::string object_output_file =
        output_file_name.substr(0, output_file_name.find_last_of('.')) + ".o";

    _output_elf32_file.write_bin(object_output_file, ET_EXEC);
    _output_elf32_file.write_hex(output_file_name);
    _output_elf32_file.read_elf(object_output_file);

    return 0;
}

void Linker::map(Elf32File& input_elf32_file)
{
    // Adds sections to out_elf32_file. If the section does not exist, create a new one and add
    // it to the output file and add it to place_arguments map so linker can change its address
    // later. If section exists in out_elf32_file then append content only.

    // NOTE: what the fuck is this
    std::map<std::string, Elf32_Off> section_addend;

    for (auto& input_it : input_elf32_file.custom_section_map) {
        const std::string& section_name = input_it.first;
        const CustomSection& section = input_it.second;

        auto output_it = _output_elf32_file.custom_section_map.find(section_name);
        if (output_it == _output_elf32_file.custom_section_map.end()) {
            _output_elf32_file.new_custom_section(section_name, section.get_header(),
                                                  section.get_data());

            if (_place_addresses.find(section_name) == _place_addresses.end()) {
                _place_addresses.emplace(section_name, 0);
            }
        }
        else {
            CustomSection& output_section = output_it->second;
            section_addend.emplace(section_name, output_section.get_size());

            output_section.append_data(section.get_data());
        }
    }

    // Map symbol table.

    std::queue<Elf32_Sym> non_section_symbols;

    for (auto symbol : input_elf32_file.symbol_table.get_symbol_table()) {
        if (ELF32_ST_TYPE(symbol.st_info) == STT_SECTION) {
            const std::string& section_name =
                input_elf32_file.string_table.get_string(symbol.st_name);

            // NOTE: there should be only one section symbol in symbol table for each section
            if (_output_elf32_file.symbol_table.get_symbol(section_name) != nullptr) {
                continue;
            }

            auto it = _output_elf32_file.custom_section_map.find(section_name);
            if (it == _output_elf32_file.custom_section_map.end()) {
                std::cerr << "Error: Section '" << section_name << "' not found in output file"
                          << std::endl;
                exit(-1);
            }

            CustomSection& section = it->second;

            symbol.st_shndx = section.get_index();
            _output_elf32_file.symbol_table.add_symbol(section_name, symbol);
        }
        else {
            non_section_symbols.push(symbol);
        }
    }

    while (!non_section_symbols.empty()) {
        Elf32_Sym new_symbol = non_section_symbols.front();
        non_section_symbols.pop();

        if (new_symbol.st_shndx != SHN_ABS) {
            const Elf32_Shdr& section_header =
                *input_elf32_file.section_header_table.at(new_symbol.st_shndx);

            const std::string& section_name =
                input_elf32_file.string_table.get_string(section_header.sh_name);

            auto addend = section_addend.find(section_name);
            if (addend != section_addend.end()) {
                new_symbol.st_value += addend->second;
            }

            new_symbol.st_shndx =
                _output_elf32_file.custom_section_map.find(section_name)->second.get_index();
        }

        const std::string& symbol_name =
            input_elf32_file.string_table.get_string(new_symbol.st_name);
        Elf32_Sym* old_symbol = _output_elf32_file.symbol_table.get_symbol(symbol_name);

        if (old_symbol != nullptr) {
            if (old_symbol->st_defined == true && new_symbol.st_defined == true) {
                std::cerr << "Error: Duplicate symbol definition in Linker::map" << std::endl;
                return;
            }
            else if (old_symbol->st_defined == false && new_symbol.st_defined == true) {
                _output_elf32_file.symbol_table.set_symbol(*old_symbol, symbol_name, new_symbol);
            }

            // NOTE: Might need to handle other cases in the future
            continue;
        }

        _output_elf32_file.symbol_table.add_symbol(symbol_name, new_symbol);
    }

    // Map relocation tables.

    for (auto& it : input_elf32_file.rela_table_map) {
        RelocationTable& in_rela_table = it.second;
        const CustomSection& in_section = in_rela_table.get_linked_section();

        std::vector<Elf32_Rela> out_rela_table_content;
        for (Elf32_Rela relocation : in_rela_table.get_relocation_table()) {

            Elf32_Sym* sym_entry =
                input_elf32_file.symbol_table.get_symbol(ELF32_R_SYM(relocation.r_info));
            if (sym_entry == nullptr) {
                std::cerr << "Error: Could not find symbol in relocation table in Linker::map"
                          << std::endl;
                return;
            }

            std::string symbol_name = input_elf32_file.string_table.get_string(sym_entry->st_name);

            relocation.r_info =
                ELF32_R_INFO(ELF32_R_TYPE(relocation.r_info),
                             _output_elf32_file.symbol_table.get_symbol_index(symbol_name));

            auto addend = section_addend.find(in_section.get_name());
            if (addend != section_addend.end()) {
                relocation.r_offset += addend->second;
            }

            out_rela_table_content.push_back(relocation);
        }

        // creates new relocation table if not exists, otherwise appends data to existing one

        auto out_rela_table_it = _output_elf32_file.rela_table_map.find(in_rela_table.get_name());
        if (out_rela_table_it == _output_elf32_file.rela_table_map.end()) {
            auto out_section_it = _output_elf32_file.custom_section_map.find(in_section.get_name());
            if (out_section_it == _output_elf32_file.custom_section_map.end()) {
                std::cerr << "Error: Could not find section for relocation table in Linker::map"
                          << std::endl;
                return;
            }
            CustomSection& out_section = out_section_it->second;

            _output_elf32_file.new_relocation_table(in_rela_table.get_name(), out_section,
                                                    in_rela_table.get_header(),
                                                    out_rela_table_content);
        }
        else {
            RelocationTable& out_rela_table = out_rela_table_it->second;
            out_rela_table.add_entry(out_rela_table_content);
        }
    }
}
