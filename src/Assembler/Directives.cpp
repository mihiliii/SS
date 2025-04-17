#include "../../inc/Assembler/Assembler.hpp"
#include "../../inc/Elf32File.hpp"
#include "misc/Exceptions.hpp"

void Assembler::Directive::section(const std::string& section_name)
{
    auto custom_section_it = elf32_file.get_custom_section_map().find(section_name);

    if (custom_section_it != elf32_file.get_custom_section_map().end()) {
        current_section = &custom_section_it->second;
    } else {
        current_section = &elf32_file.new_custom_section(section_name);
        elf32_file.get_symbol_table().add_symbol(current_section->get_name(), 0, true,
                                                 current_section->get_index(),
                                                 ELF32_ST_INFO(STB_LOCAL, STT_SECTION));
    }

    literal_table_map.emplace(current_section, LiteralTable(&elf32_file, current_section));
}

void Assembler::Directive::skip(int _bytes)
{
    std::vector<char> vector(_bytes, 0);
    current_section->add_data(vector.data(), _bytes);
}

void Assembler::Directive::word(const std::vector<Operand>& _values)
{
    for (const Operand& node : _values) {
        if (node.type == typeid(uint32_t).name())
            current_section->add_data(node.value, sizeof(int));
        if (node.type == typeid(char*).name()) {
            std::string symbol_name = std::string((char*) node.value);
            Elf32_Sym* symbol_entry = elf32_file.get_symbol_table().get_symbol(symbol_name);

            if (symbol_entry == nullptr) {
                symbol_entry = &elf32_file.get_symbol_table().add_symbol(
                    symbol_name, 0, false, current_section->get_index());
            }

            uint32_t symbol_entry_index =
                elf32_file.get_symbol_table().get_symbol_index(*symbol_entry);

            current_section->get_relocation_table().add_entry(
                current_section->get_size(), ELF32_R_INFO(ELF32_R_TYPE_ABS32, symbol_entry_index),
                0);

            current_section->add_data((instruction_t) 0);
        }
    }
}

void Assembler::Directive::global_symbol(const std::vector<Operand>& _symbols)
{
    for (const Operand& node : _symbols) {
        std::string symbol_name = std::string((char*) node.value);
        Elf32_Sym* symbol_entry = elf32_file.get_symbol_table().get_symbol(symbol_name);

        if (symbol_entry == nullptr) {
            elf32_file.get_symbol_table().add_symbol(symbol_name, 0, false, SHN_ABS,
                                                     ELF32_ST_INFO(STB_GLOBAL, STT_NOTYPE));
        } else {
            Elf32_Half type = ELF32_ST_TYPE(symbol_entry->st_info);
            elf32_file.get_symbol_table().get_symbol(symbol_name)->st_info =
                ELF32_ST_INFO(STB_GLOBAL, type);
        }
    }
}

void Assembler::Directive::extern_symbol(const std::vector<Operand>& _symbols)
{
    for (const Operand& node : _symbols) {
        std::string symbol_name = std::string((char*) node.value);
        Elf32_Sym* symbol_entry = elf32_file.get_symbol_table().get_symbol(symbol_name);

        if (symbol_entry == nullptr) {
            elf32_file.get_symbol_table().add_symbol(symbol_name, 0, false, SHN_ABS,
                                                     ELF32_ST_INFO(STB_GLOBAL, STT_NOTYPE));
        } else {
            Elf32_Half type = ELF32_ST_TYPE(symbol_entry->st_info);
            elf32_file.get_symbol_table().get_symbol(symbol_name)->st_info =
                ELF32_ST_INFO(STB_GLOBAL, type);
        }
    }
}

int Assembler::Directive::label(const std::string& _label)
{
    Elf32_Sym* symbol_entry = elf32_file.get_symbol_table().get_symbol(_label);
    Elf32_Off location_counter = current_section->get_size();

    if (symbol_entry != nullptr)
        if (symbol_entry->st_defined == true) {
            const std::string& symbol_name =
                elf32_file.get_string_table().get_string(symbol_entry->st_name);
            THROW_EXCEPTION("Symbol " + symbol_name + "already defined");
        } else {
            elf32_file.get_symbol_table().define_symbol(*symbol_entry, location_counter,
                                                        current_section->get_index());
        }
    else {
        symbol_entry = &elf32_file.get_symbol_table().add_symbol(_label, location_counter, true,
                                                                 current_section->get_index());
    }

    return 0;
}
