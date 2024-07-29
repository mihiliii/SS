#include "../inc/Section.hpp"

#include "../inc/SectionHeaderTable.hpp"
#include "../inc/SectionHeaderStringTable.hpp"
#include "Section.hpp"

template class Section<char>;
template class Section<Elf32_Sym>;

template <typename T>
Section<T>::Section(const std::string& _name) : content({}), section_header(nullptr) {
    SectionHeaderTable::getInstance().insert(&section_header);
    SectionHeaderStringTable::getInstance().setSectionName(this, _name);
}

template <typename T>
Section<T>::Section() {
    SectionHeaderTable::getInstance().insert(&section_header);
}
