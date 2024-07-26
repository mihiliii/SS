#ifndef SectionHeaderStringTable_hpp_
#define SectionHeaderStringTable_hpp_

#include "Elf32.hpp"
#include "Section.hpp"

class SectionHeaderStringTable : public Section<char> {
public:

    ~SectionHeaderStringTable() = default;

    void appendContent(char* _content, size_t _size);

    Elf32_Word setSectionName(const std::string& _content);

    static SectionHeaderStringTable& getInstance();

private:

    SectionHeaderStringTable();

    SectionHeaderStringTable(const SectionHeaderStringTable&) = delete;

    SectionHeaderStringTable& operator=(const SectionHeaderStringTable&) = delete;
};

#endif
