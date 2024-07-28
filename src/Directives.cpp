#include "../inc/Directives.hpp"
#include "../inc/Section.hpp"
#include "../inc/Assembler.hpp"

void Directives::dSection(const std::string& _section_name) {
    Assembler::sections.emplace_back(Section<char>(_section_name));
    Assembler::resetLocationCounter();
}
