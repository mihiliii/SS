#include "../inc/Directives.hpp"

#include <exception>

#include "../inc/Assembler.hpp"
#include "../inc/CustomSection.hpp"
#include "../inc/Section.hpp"
#include "../inc/SymbolTable.hpp"

void Directives::dSection(const std::string& _section_name) {
    CustomSection* section = new CustomSection(_section_name);
    Assembler::current_section = section;
}

void Directives::dSkip(int _bytes) {
    std::vector<char> vector(_bytes, 0);
    Assembler::current_section->appendContent(vector.data(), _bytes);
}

void Directives::dWord(std::vector<init_list_node>* _values) {
    CustomSection* current_section = Assembler::current_section;
    SymbolTable* symbol_table = Assembler::symbol_table;

    for (init_list_node& node : *_values) {
        if (node.type == typeid(int).name())
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

void Directives::dGlobal(std::vector<init_list_node>* _symbols) {
    SymbolTable* symbol_table = Assembler::symbol_table;
    for (init_list_node& node : *_symbols) {
        std::string symbol_name = std::string((char*) node.value);
        Elf32_Sym* symbol_entry = symbol_table->findSymbol(symbol_name);
        // if symbol is not in symbol table
        if (symbol_entry == nullptr)
            symbol_table->addSymbol(symbol_name, 0, false);
        symbol_table->setInfo(symbol_name, STB_GLOBAL);
    }
}

void Directives::dExtern(std::vector<init_list_node>* _symbols) {
    SymbolTable* symbol_table = Assembler::symbol_table;
    for (init_list_node& node : *_symbols) {
        std::string symbol_name = std::string((char*) node.value);
        Elf32_Sym* symbol_entry = symbol_table->findSymbol(symbol_name);
        // if symbol is not in symbol table
        if (symbol_entry == nullptr)
            symbol_table->addSymbol(symbol_name, 0, false);
        symbol_table->setInfo(symbol_name, STB_EXTERN);
    }
}
