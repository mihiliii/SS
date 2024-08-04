#include "../inc/Directives.hpp"
#include "../inc/Section.hpp"
#include "../inc/Assembler.hpp"
#include "../inc/InputSection.hpp"   

void Directives::dSection(const std::string& _section_name) {
    InputSection<char>* section = new InputSection<char>(_section_name);
    Assembler::current_section = section;
    Assembler::resetLocationCounter();
}

void Directives::dEnd() {
    Assembler::increaseLocationCounter();
}

void Directives::dSkip(int _bytes) {
    for (int i = 0; i < _bytes; i++) {
        dynamic_cast<InputSection<char>*> (Assembler::current_section)->appendContent(0);
    }
}
