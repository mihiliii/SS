#pragma once

#include <string>

#include "Section.hpp"
#include "SectionHeaderStringTable.hpp"
#include "SectionHeaderTable.hpp"

template <typename T>
class InputSection : public Section {
public:

    InputSection(const std::string& _name);

    void appendContent(const T& _content);

    void appendContent(T* _content, size_t _size);

    ~InputSection() = default;

private:

    std::vector<T> content;

};

template <typename T>
inline InputSection<T>::InputSection(const std::string& _name) : Section() {
    SectionHeaderStringTable::getInstance().setSectionName(this, _name);
    this->name = _name;
}

template <typename T>
inline void InputSection<T>::appendContent(const T& _content) {
    this->content.push_back(_content);
    this->section_header.sh_size += sizeof(T);
}

template <typename T>
inline void InputSection<T>::appendContent(T* _content, size_t _size) {
    for (size_t i = 0; i < _size; i++) {
        this->content.push_back(_content[i]);
    }
    this->section_header.sh_size += _size * sizeof(T);
}


