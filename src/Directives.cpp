#include "../inc/Directives.hpp"

#include <exception>

#include "../inc/Assembler.hpp"
#include "../inc/InputSection.hpp"
#include "../inc/Section.hpp"

void Directives::dSection(const std::string& _section_name) {
    InputSection* section = new InputSection(_section_name);
    Assembler::current_section = section;
    Assembler::resetLocationCounter();
}

void Directives::dEnd() {}

void Directives::dSkip(int _bytes) {
    dynamic_cast<InputSection*>(Assembler::current_section)->appendContent((char) 0, _bytes);
}