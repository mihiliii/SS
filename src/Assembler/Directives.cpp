#include "../../inc/Assembler/Directives.hpp"

#include <exception>

#include "../../inc/Assembler/Assembler.hpp"
#include "../../inc/Assembler/ForwardReferenceTable.hpp"
#include "../../inc/CustomSection.hpp"
#include "../../inc/Elf32File.hpp"
#include "../../inc/Section.hpp"
#include "../../inc/SymbolTable.hpp"
#include "../../inc/LiteralTable.hpp"

void Directives::sectionDirective(const std::string& _section_name) {
    CustomSectionMap::iterator it_custom_section = Assembler::elf32_file->customSectionMap().find(_section_name);
    if (it_custom_section != Assembler::elf32_file->customSectionMap().end()) {
        Assembler::current_section = it_custom_section->second;
    }
    else {
        CustomSection* section = new CustomSection(Assembler::elf32_file, _section_name);
        Assembler::elf32_file->symbolTable().add(
            section->name(), 0, true, section->index(), ELF32_ST_INFO(STB_LOCAL, STT_SECTION)
        );
        Assembler::current_section = section;
    }
    Assembler::literal_table_map.insert(std::pair<CustomSection*, LiteralTable>(
        Assembler::current_section, LiteralTable(Assembler::elf32_file, Assembler::current_section)
    ));
}

void Directives::skipDirective(int _bytes) {
    std::vector<char> vector(_bytes, 0);
    Assembler::current_section->append(vector.data(), _bytes);
}

void Directives::wordDirective(std::vector<Operand>* _values) {
    CustomSection* current_section = Assembler::current_section;
    SymbolTable* symbol_table = &Assembler::elf32_file->symbolTable();

    for (Operand& node : *_values) {
        if (node.type == typeid(uint32_t).name())
            current_section->append(node.value, sizeof(int));
        if (node.type == typeid(char*).name()) {
            std::string symbol_name = std::string((char*) node.value);
            Elf32_Sym* symbol_entry = symbol_table->get(symbol_name);

            // if symbol is not in symbol table
            if (symbol_entry == nullptr) {
                symbol_entry = symbol_table->add(symbol_name, 0, false, Assembler::current_section->index());
            }

            uint32_t symbol_entry_index = symbol_table->getIndex(symbol_entry);

            current_section->relocationTable()->add(
                current_section->size(), ELF32_R_INFO(ELF32_R_ABS32, symbol_entry_index), 0
            );

            current_section->append((instruction_format) 0);
        }
    }
}

void Directives::globalDirective(std::vector<Operand>* _symbols) {
    SymbolTable* symbol_table = &Assembler::elf32_file->symbolTable();

    for (Operand& node : *_symbols) {
        std::string symbol_name = std::string((char*) node.value);
        Elf32_Sym* symbol_entry = symbol_table->get(symbol_name);

        if (symbol_entry == nullptr) {
            symbol_table->add(symbol_name, 0, false, SHN_ABS, ELF32_ST_INFO(STB_GLOBAL, STT_NOTYPE));
        }
        else {
            Elf32_Half type = ELF32_ST_TYPE(symbol_entry->st_info);
            symbol_table->get(symbol_name)->st_info = ELF32_ST_INFO(STB_GLOBAL, type);
        }
    }
}

void Directives::externDirective(std::vector<Operand>* _symbols) {
    SymbolTable* symbol_table = &Assembler::elf32_file->symbolTable();
    for (Operand& node : *_symbols) {
        std::string symbol_name = std::string((char*) node.value);
        Elf32_Sym* symbol_entry = symbol_table->get(symbol_name);

        if (symbol_entry == nullptr) {
            symbol_table->add(symbol_name, 0, false, SHN_ABS, ELF32_ST_INFO(STB_GLOBAL, STT_NOTYPE));
        }
        else {
            Elf32_Half type = ELF32_ST_TYPE(symbol_entry->st_info);
            symbol_table->get(symbol_name)->st_info = ELF32_ST_INFO(STB_GLOBAL, type);
        }
    }
}

int Directives::defineLabel(std::string _label) {
    Elf32_Sym* symbol_entry = Assembler::elf32_file->symbolTable().get(_label);
    Elf32_Off location_counter = Assembler::current_section->size();
    if (symbol_entry != nullptr)
        if (symbol_entry->st_defined == true) {
            std::cout << "Symbol " << _label << " already defined!" << std::endl;
            return -1;
        }
        else {
            Assembler::elf32_file->symbolTable().defineSymbol(symbol_entry, location_counter);
        }
    else {
        symbol_entry = Assembler::elf32_file->symbolTable().add(
            _label, location_counter, true, Assembler::current_section->index()
        );
    }

    return 0;
}
