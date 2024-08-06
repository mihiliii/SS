#pragma once

#include <vector>

#include "Elf32.hpp"
#include "Section.hpp"

class SectionHeaderStringTable : Section {
public:

    static SectionHeaderStringTable& getInstance();

    void setSectionName(Section* _section);

    void printContent() const;

    SectionHeaderStringTable(const SectionHeaderStringTable&) = delete;
    SectionHeaderStringTable& operator=(const SectionHeaderStringTable&) = delete;

    ~SectionHeaderStringTable() = default;

private:

    std::vector<char> content;

    SectionHeaderStringTable();
};

