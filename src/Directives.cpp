#include "../inc/Directives.hpp"

#include <exception>

#include "../inc/Assembler.hpp"
#include "../inc/InputSection.hpp"
#include "../inc/Section.hpp"
#include "../inc/SymbolTable.hpp"

void Directives::dSection(const std::string& _section_name) {
    InputSection* section = new InputSection(_section_name);
    Assembler::current_section = section;
    Assembler::resetLocationCounter();
}

void Directives::dEnd() {}

void Directives::dSkip(int _bytes) {
    std::vector<char> vector(_bytes, 0);
    dynamic_cast<InputSection*>(Assembler::current_section)->appendContent(vector.data(), _bytes);
}

void Directives::dWord(std::vector<init_list_node>* _values) {
    for (init_list_node& value : *_values) {
        if (value.type == typeid(int).name()) {
            dynamic_cast<InputSection*>(Assembler::current_section)->appendContent((char*) value.value, sizeof(int));
            dynamic_cast<InputSection*>(Assembler::current_section)->printContent();
        } else if (value.type == typeid(std::string).name()) {
            SymbolTable::getInstance().addSymbol(*(std::string*) value.value, Assembler::location_counter);
            dynamic_cast<InputSection*>(Assembler::current_section)->printContent();
        }
    }
}
