#pragma once

#include <vector>

#include "Elf32.hpp"
#include "Section.hpp"

class SectionHeaderStringTable : public Section {
public:

    static SectionHeaderStringTable& getInstance();

    void setSectionName(Section* _section, const std::string& _content);

    std::string getSectionName(Section* _section) const;

    void printContent() const;

    SectionHeaderStringTable(const SectionHeaderStringTable&) = delete;
    SectionHeaderStringTable& operator=(const SectionHeaderStringTable&) = delete;

    ~SectionHeaderStringTable() = default;

private:

    std::vector<char> content;

    SectionHeaderStringTable();
};

