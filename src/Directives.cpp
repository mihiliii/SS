#include "../inc/Directives.hpp"

#include <exception>

#include "../inc/Assembler.hpp"
#include "../inc/CustomSection.hpp"
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

void Directives::wordDirective(std::vector<operand>* _values) {
    CustomSection* current_section = Assembler::current_section;
    SymbolTable* symbol_table = Assembler::symbol_table;

    for (operand& node : *_values) {
        if (node.type == typeid(uint32_t).name())
            current_section->appendContent(node.value, sizeof(int));
        if (node.type == typeid(char*).name()) {
            std::string symbol_name = std::string((char*) node.value);
            Elf32_Sym* symbol_entry = symbol_table->findSymbol(symbol_name);

            // if symbol is not in symbol table
            if (symbol_entry == nullptr) {
                symbol_entry = symbol_table->addSymbol(symbol_name, 0, false);
                symbol_table->addSymbolReference(symbol_entry, Assembler::current_section->getLocationCounter());
                uint32_t zero = 0;
                current_section->appendContent(&zero, sizeof(uint32_t));
            }
            else {
                // if symbol is in symbol table but not defined
                if (symbol_entry->st_defined == false) {
                    symbol_table->addSymbolReference(symbol_entry, current_section->getLocationCounter());
                    uint32_t zero = 0;
                    current_section->appendContent(&zero, sizeof(uint32_t));
                }
                // if symbol is in symbol table and defined
                else if (symbol_entry->st_defined == true) {
                    current_section->appendContent(&symbol_table->findSymbol(symbol_name)->st_value, sizeof(int));
                }
            }
        }
    }
}

void Directives::globalDirective(std::vector<operand>* _symbols) {
    SymbolTable* symbol_table = Assembler::symbol_table;
    for (operand& node : *_symbols) {
        std::string symbol_name = std::string((char*) node.value);
        Elf32_Sym* symbol_entry = symbol_table->findSymbol(symbol_name);
        // if symbol is not in symbol table
        if (symbol_entry == nullptr)
            symbol_entry = symbol_table->addSymbol(symbol_name, 0, false);
        Elf32_Half type = ELF32_ST_TYPE(symbol_entry->st_info);
        symbol_table->setInfo(symbol_name, ELF32_ST_INFO(STB_GLOBAL, type));
    }
}

void Directives::externDirective(std::vector<operand>* _symbols) {
    SymbolTable* symbol_table = Assembler::symbol_table;
    for (operand& node : *_symbols) {
        std::string symbol_name = std::string((char*) node.value);
        Elf32_Sym* symbol_entry = symbol_table->findSymbol(symbol_name);
        // if symbol is not in symbol table
        if (symbol_entry == nullptr)
            symbol_entry = symbol_table->addSymbol(symbol_name, 0, false);
        Elf32_Half type = ELF32_ST_TYPE(symbol_entry->st_info);
        symbol_table->setInfo(symbol_name, ELF32_ST_INFO(STB_GLOBAL, type));
    }
}

int Directives::defineLabel(std::string _label) {
    Elf32_Sym* symbol_entry = Assembler::symbol_table->findSymbol(_label);
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
        Assembler::symbol_table->addSymbol(_label, location_counter, true);
    }

    Assembler::current_section->getRelocationTable().add(
        location_counter, Assembler::current_section->getSectionHeaderTableIndex(), 0
    );

    return 0;
}
