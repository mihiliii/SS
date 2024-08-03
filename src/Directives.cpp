#include "../inc/Directives.hpp"
#include "../inc/Section.hpp"
#include "../inc/Assembler.hpp"
#include "../inc/InputSection.hpp"   

void Directives::dSection(const std::string& _section_name) {
    InputSection<char>* section = new InputSection<char>(_section_name);
    Assembler::resetLocationCounter();
}
