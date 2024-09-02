#include "../inc/Directives.hpp"

#include <exception>

#include "../inc/Assembler.hpp"
#include "../inc/CustomSection.hpp"
#include "../inc/ForwardReferenceTable.hpp"
#include "../inc/Section.hpp"
#include "../inc/SymbolTable.hpp"

void Directives::sectionDirective(const std::string& _section_name) {
    CustomSection* section = new CustomSection(_section_name);
    Assembler::current_section = section;
}

void Directives::skipDirective(int _bytes) {
    std::vector<char> vector(_bytes, 0);
    Assembler::current_section->appendContent(vector.data(), _bytes);
}

void Directives::wordDirective(std::vector<Operand>* _values) {
    CustomSection* current_section = Assembler::current_section;
    SymbolTable* symbol_table = Assembler::symbol_table;

    for (Operand& node : *_values) {
        if (node.type == typeid(uint32_t).name())
            current_section->appendContent(node.value, sizeof(int));
        if (node.type == typeid(char*).name()) {
            std::string symbol_name = std::string((char*) node.value);
            Elf32_Sym* symbol_entry = symbol_table->getSymbol(symbol_name);

            // if symbol is not in symbol table
            if (symbol_entry == nullptr) {
                symbol_entry = symbol_table->addSymbol(symbol_name, 0, false);
            }

            uint32_t symbol_entry_index = symbol_table->getSymbolEntryIndex(symbol_entry);

            current_section->getRelocationTable().add(
                current_section->getLocationCounter(), ELF32_R_INFO(ELF32_R_ABS32, symbol_entry_index), 0
            );

            current_section->appendContent((instruction_format) 0);
        }
    }
}

void Directives::globalDirective(std::vector<Operand>* _symbols) {
    SymbolTable* symbol_table = Assembler::symbol_table;
    for (Operand& node : *_symbols) {
        std::string symbol_name = std::string((char*) node.value);
        Elf32_Sym* symbol_entry = symbol_table->getSymbol(symbol_name);

        if (symbol_entry == nullptr) {
            symbol_table->addSymbol(symbol_name, 0, false, SHN_ABS, ELF32_ST_INFO(STB_GLOBAL, STT_NOTYPE));
        }
        else {
            Elf32_Half type = ELF32_ST_TYPE(symbol_entry->st_info);
            symbol_table->setInfo(symbol_name, ELF32_ST_INFO(STB_GLOBAL, type));
        }
    }
}

void Directives::externDirective(std::vector<Operand>* _symbols) {
    SymbolTable* symbol_table = Assembler::symbol_table;
    for (Operand& node : *_symbols) {
        std::string symbol_name = std::string((char*) node.value);
        Elf32_Sym* symbol_entry = symbol_table->getSymbol(symbol_name);

        if (symbol_entry == nullptr) {
            symbol_table->addSymbol(symbol_name, 0, false, SHN_ABS, ELF32_ST_INFO(STB_GLOBAL, STT_NOTYPE));
        }
        else {
            Elf32_Half type = ELF32_ST_TYPE(symbol_entry->st_info);
            symbol_table->setInfo(symbol_name, ELF32_ST_INFO(STB_GLOBAL, type));
        }
    }
}

int Directives::defineLabel(std::string _label) {
    Elf32_Sym* symbol_entry = Assembler::symbol_table->getSymbol(_label);
    Elf32_Off location_counter = Assembler::current_section->getLocationCounter();
    if (symbol_entry != nullptr)
        if (symbol_entry->st_defined == true) {
            std::cout << "Symbol " << _label << " already defined!" << std::endl;
            return -1;
        }
        else {
            Assembler::symbol_table->defineSymbol(symbol_entry, location_counter);
        }
    else {
        symbol_entry = Assembler::symbol_table->addSymbol(_label, location_counter, true);
    }

    return 0;
}
