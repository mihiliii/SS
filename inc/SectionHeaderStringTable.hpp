#ifndef SectionHeaderStringTable_hpp_
#define SectionHeaderStringTable_hpp_

#include <Section.hpp>
#include "Elf32.hpp"

class SectionHeaderStringTable : public Section<char> {
public:

    SectionHeaderStringTable() : Section(".shstrtab") {
        section_header.sh_type = SHT_SHSTRTAB;
    }

    ~SectionHeaderStringTable() {}

    void appendContent(char* _content, size_t _size) {
        std::string content(_content);
        setSectionName(_content);
    }

    Elf32_Word setSectionName(const std::string& _content) {
        int offset = section_header.sh_size;
        for (char c : _content) {
            content.push_back(c);
        }
        content.push_back('\0');
        section_header.sh_size += _content.size() + 1;
        return offset;
    }

};

#endif
