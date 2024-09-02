#pragma once

#include <iostream>
#include <string>
#include <typeinfo>

#include "Assembler.hpp"

struct Operand;

class Directives {
public:

    friend class Assembler;

    static void sectionDirective(const std::string& _section_name);

    static void skipDirective(int _bytes);

    static void wordDirective(std::vector<Operand>* _values);

    static void globalDirective(std::vector<Operand>* _symbols);

    static void externDirective(std::vector<Operand>* _symbols);

    static int defineLabel(std::string _label);
};
