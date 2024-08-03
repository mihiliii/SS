#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "Elf32.hpp"
#include "SectionHeaderTable.hpp"


template <typename T>
class Section {
public:

    virtual ~Section() = 0;

    Elf32_Shdr& getSectionHeader() { return section_header; }

    void getName() const;

protected:

    Section();

    void addObjectToGarbageCollector(Section* _object);

    std::vector<T> content;
    Elf32_Shdr section_header;
    std::string name;
    
    static std::vector<Section<T>*> garbage_collector;
};

template <typename T>
Section<T>::Section() : content(), section_header() {
    SectionHeaderTable::getInstance().insert(&section_header);
}

template <typename T>
void Section<T>::addObjectToGarbageCollector(Section* _object) {
    garbage_collector.push_back(_object);
}

template <typename T>
inline void Section<T>::getName() const {
    std::cout << name;
}

template <typename T>
Section<T>::~Section() {
    for (auto it = garbage_collector.begin(); it != garbage_collector.end(); it++) {
        getName();
        if (*it == this) {
            garbage_collector.erase(it);
        }
    }
}

template <typename T>
std::vector<Section<T>*> Section<T>::garbage_collector;
