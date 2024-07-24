#ifndef Section_hpp_
#define Section_hpp_

#include <iostream>
#include <string>
#include <vector>

#include "Elf32.hpp"
#include "Section.hpp"

template <typename T>
class Section {
public:

    Section(const std::string& _name)
        : content({}),
          section_header(
              {.sh_name = Assembler::section_header_string_table->setSectionName(_sh_name),
               .sh_type = 0,
               .sh_flags = 0,
               .sh_addr = 0,
               .sh_offset = 0,
               .sh_size = 0,
               .sh_link = 0,
               .sh_info = 0,
               .sh_addralign = 0,
               .sh_entsize = 0}
          ) {
        Assembler::section_header_table->insertSectionEntry(section_header);
    }

    ~Section() {}

    void appendContent(T* _content, size_t _size);

    void setName(const std::string& _name);

    void setType(Elf32_Word _type);

    void printContent();

    std::vector<T>* getContent() const { return &content; }

protected:

    std::vector<T> content;
    Elf32_Shdr section_header;
};

#endif

template <typename T>
inline void Section<T>::appendContent(T* _content, size_t _size) {
    for (size_t i = 0; i < _size; i++) {
        content.push_back(_content[i]);
    }
    section_header.sh_size += _size;
}

template <typename T>
inline void Section<T>::setName(const std::string& _name) {
    section_header.sh_name = Assembler::section_header_string_table->setSectionName(_name);
}

template <typename T>
inline void Section<T>::setType(Elf32_Word _type) {
    section_header.sh_type = _type;
}

template <typename T>
inline void Section<T>::printContent() {
    for (T t : content) {
        std::cout << t << std::endl;
    }
}


