#pragma once

#include <iostream>
#include <string>
#include <typeinfo>

#include "Assembler.hpp"

struct operand;

class Directives {
public:

    friend class Assembler;

    static void sectionDirective(const std::string& _section_name);

    static void skipDirective(int _bytes);

    static void wordDirective(std::vector<operand>* _values);

    static void globalDirective(std::vector<operand>* _symbols);

    static void externDirective(std::vector<operand>* _symbols);

    static int defineLabel(std::string _label);
};
