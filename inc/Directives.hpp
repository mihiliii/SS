#ifndef Directives_hpp_
#define Directives_hpp_

#include <iostream>
#include <string>

class Directives {
public:

    friend class Assembler;

    static void dSection(const std::string& _section_name);

};

#endif