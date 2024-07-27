#ifndef SectionHeaderStringTable_hpp_
#define SectionHeaderStringTable_hpp_

#include "Elf32.hpp"
#include "Section.hpp"

class SectionHeaderStringTable : public Section<char> {
public:

    Elf32_Word setSectionName(const std::string& _content);

    static SectionHeaderStringTable& getInstance();

    SectionHeaderStringTable(const SectionHeaderStringTable&) = delete;
    SectionHeaderStringTable& operator=(const SectionHeaderStringTable&) = delete;

    ~SectionHeaderStringTable() = default;

private:

    SectionHeaderStringTable();
};

#endif
