#include "../inc/Section.hpp"

#include "../inc/Assembler.hpp"
#include "../inc/SectionHeaderStringTable.hpp"
#include "../inc/SectionHeaderTable.hpp"

template class Section<char>;
template class Section<Elf32_Sym>;

template <typename T>
Section<T>::Section(const std::string& _name) : content({}), section_header(nullptr) {
    SectionHeaderTable::getInstance().insertSectionEntry(&section_header);
}

template <typename T>
inline void Section<T>::appendContent(T* _content, size_t _size) {
    for (size_t i = 0; i < _size; i++) {
        content.emplace_back(_content[i]);
    }
    section_header->sh_size += _size;
}

template <typename T>
inline void Section<T>::printContent() {
    for (T t : content) {
        std::cout << t;
    }
}

template <>
inline void Section<Elf32_Sym>::printContent() {
    for (Elf32_Sym t : content) {
        std::cout << "st_name: " << t.st_name << std::endl;
        std::cout << "st_value: " << t.st_value << std::endl;
        std::cout << "st_size: " << t.st_size << std::endl;
        std::cout << "st_info: " << t.st_info << std::endl;
        std::cout << "st_shndx: " << t.st_shndx << std::endl;
    }
}
