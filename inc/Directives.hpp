#pragma once

#include <iostream>
#include <string>

#include "Assembler.hpp"

class Directives {
public:

    friend class Assembler;

    static void dSection(const std::string& _section_name);

    static void dEnd();

    static void dSkip(int _bytes);

};
