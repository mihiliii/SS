#include "../../inc/Assembler/Directives.hpp"

#include "../../inc/Assembler/Assembler.hpp"
#include "../../inc/Assembler/ForwardReferenceTable.hpp"
#include "../../inc/Assembler/LiteralTable.hpp"
#include "../../inc/Elf32/Elf32File.hpp"

CustomSection*& Directives::current_section = Assembler::current_section;
Elf32File& Directives::elf32_file = Assembler::elf32_file;
ForwardReferenceTable& Directives::forward_reference_table = Assembler::forward_reference_table;
std::map<CustomSection*, LiteralTable>& Directives::literal_table_map =
    Assembler::literal_table_map;

void Directives::sectionDirective(const std::string& _section_name) {
    CustomSectionMap::iterator custom_section_iterator =
        elf32_file.customSectionMap().find(_section_name);

    if (custom_section_iterator != elf32_file.customSectionMap().end()) {
        current_section = &custom_section_iterator->second;
    } else {
        current_section = elf32_file.newCustomSection(_section_name);
        elf32_file.symbolTable().add(current_section->name(), 0, true, current_section->index(),
                                     ELF32_ST_INFO(STB_LOCAL, STT_SECTION));
    }

    literal_table_map.emplace(current_section, LiteralTable(&elf32_file, current_section));
}

void Directives::skipDirective(int _bytes) {
    std::vector<char> vector(_bytes, 0);
    current_section->append(vector.data(), _bytes);
}

void Directives::wordDirective(std::vector<Operand>* _values) {
    for (Operand& node : *_values) {
        if (node.type == typeid(uint32_t).name()) {
            current_section->append(node.value, sizeof(int));
        }
        if (node.type == typeid(char*).name()) {
            std::string symbol_name = std::string((char*) node.value);
            Elf32_Sym* symbol_entry = elf32_file.symbolTable().get(symbol_name);

            if (symbol_entry == nullptr) {
                symbol_entry =
                    &elf32_file.symbolTable().add(symbol_name, 0, false, current_section->index());
            }

            uint32_t symbol_entry_index = elf32_file.symbolTable().getIndex(*symbol_entry);

            current_section->relocationTable().add(
                current_section->size(), ELF32_R_INFO(ELF32_R_TYPE_ABS32, symbol_entry_index), 0);

            current_section->append((instruction_format_t) 0);
        }
    }
}

void Directives::globalDirective(std::vector<Operand>* _symbols) {
    for (Operand& node : *_symbols) {
        std::string symbol_name = std::string((char*) node.value);
        Elf32_Sym* symbol_entry = elf32_file.symbolTable().get(symbol_name);

        if (symbol_entry == nullptr) {
            elf32_file.symbolTable().add(symbol_name, 0, false, SHN_ABS,
                                         ELF32_ST_INFO(STB_GLOBAL, STT_NOTYPE));
        } else {
            Elf32_Half type = ELF32_ST_TYPE(symbol_entry->st_info);
            elf32_file.symbolTable().get(symbol_name)->st_info = ELF32_ST_INFO(STB_GLOBAL, type);
        }
    }
}

void Directives::externDirective(std::vector<Operand>* _symbols) {
    for (Operand& node : *_symbols) {
        std::string symbol_name = std::string((char*) node.value);
        Elf32_Sym* symbol_entry = elf32_file.symbolTable().get(symbol_name);

        if (symbol_entry == nullptr) {
            elf32_file.symbolTable().add(symbol_name, 0, false, SHN_ABS,
                                         ELF32_ST_INFO(STB_GLOBAL, STT_NOTYPE));
        } else {
            Elf32_Half type = ELF32_ST_TYPE(symbol_entry->st_info);
            elf32_file.symbolTable().get(symbol_name)->st_info = ELF32_ST_INFO(STB_GLOBAL, type);
        }
    }
}

int Directives::defineLabel(std::string _label) {
    Elf32_Sym* symbol_entry = elf32_file.symbolTable().get(_label);
    Elf32_Off location_counter = current_section->size();

    if (symbol_entry != nullptr) {
        if (symbol_entry->st_defined == true) {
            std::cout << "Symbol " << _label << " already defined!" << std::endl;
            return -1;
        } else {
            elf32_file.symbolTable().defineSymbol(symbol_entry, location_counter,
                                                  current_section->index());
        }
    } else {
        symbol_entry =
            &elf32_file.symbolTable().add(_label, location_counter, true, current_section->index());
    }

    return 0;
}
