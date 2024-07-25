#ifndef SectionHeaderStringTable_hpp_
#define SectionHeaderStringTable_hpp_

#include "Section.hpp"
#include "Elf32.hpp"

class SectionHeaderStringTable : public Section<char> {
public:

    ~SectionHeaderStringTable() {}

    void appendContent(char* _content, size_t _size);

    Elf32_Word setSectionName(const std::string& _content); 

    static SectionHeaderStringTable& getInstance(); 
private:

    SectionHeaderStringTable() : Section() {
        section_header.sh_name = setSectionName(".shstrtab");
        section_header.sh_type = SHT_SHSTRTAB;
    }

    SectionHeaderStringTable(const SectionHeaderStringTable&) = delete;

    SectionHeaderStringTable& operator=(const SectionHeaderStringTable&) = delete;

};

#endif
