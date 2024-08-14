#include "../inc/Directives.hpp"

#include <exception>

#include "../inc/Assembler.hpp"
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
        if (node.type == typeid(int).name()) {
            current_section->appendContent(node.value, sizeof(int));
        } else if (node.type == typeid(std::string).name()) {
            if (symbol_table.findSymbol(*(std::string*) node.value) != nullptr) {
                current_section->appendContent(
                    &symbol_table.findSymbol(*(std::string*) node.value)->st_value, sizeof(int)
                );
            } else {
                symbol_table.addSymbol(*(std::string*) node.value, current_section->getLocationCounter());
                std::vector<char> vector(sizeof(uint32_t), 0);
                current_section->appendContent(vector.data(), sizeof(uint32_t));
            }
        }
    }
}
