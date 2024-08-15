#include "../inc/Directives.hpp"

#include <exception>

#include "../inc/Assembler.hpp"
#include "../inc/ForwardReferenceTable.hpp"
#include "../inc/InputSection.hpp"
#include "../inc/Section.hpp"
#include "../inc/SymbolTable.hpp"

void Directives::dSection(const std::string& _section_name) {
    InputSection* section = new InputSection(_section_name);
    Assembler::current_section = section;
}

void Directives::dEnd() {}

void Directives::dSkip(int _bytes) {
    InputSection* current_section = dynamic_cast<InputSection*>(Assembler::current_section);
    std::vector<char> vector(_bytes, 0);
    current_section->appendContent(vector.data(), _bytes);
}

void Directives::dWord(std::vector<init_list_node>* _values) {
    InputSection* current_section = dynamic_cast<InputSection*>(Assembler::current_section);
    SymbolTable& symbol_table = SymbolTable::getInstance();
    for (init_list_node& node : *_values) {
        if (node.type == typeid(int).name()) current_section->appendContent(node.value, sizeof(int));
        if (node.type == typeid(std::string).name()) {
            Elf32_Sym* symbol_entry = symbol_table.findSymbol(*(std::string*) node.value);
            // if symbol is not in symbol table
            if (symbol_entry == nullptr) {
                symbol_table.addSymbol(*(std::string*) node.value, 0, false);
                uint32_t zero = 0;
                current_section->appendContent(&zero, sizeof(uint32_t));
            } else {
                // if symbol is in symbol table but not defined
                if (symbol_entry->st_defined == false) {
                    ForwardReferenceTable::getInstance().addReference(
                        symbol_entry, current_section->getLocationCounter()
                    );
                    uint32_t zero = 0;
                    current_section->appendContent(&zero, sizeof(uint32_t));
                }
                // if symbol is in symbol table and defined
                else if (symbol_entry->st_defined == true) {
                    current_section->appendContent(
                        &symbol_table.findSymbol(*(std::string*) node.value)->st_value, sizeof(int)
                    );
                }
            }
        }
    }
}
