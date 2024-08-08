#pragma once

#include <iostream>
#include <string>
#include <typeinfo>

#include "Assembler.hpp"

struct init_list_node {
    std::string type;
    void* value;
};

class Directives {
public:

    friend class Assembler;

    static void dSection(const std::string& _section_name);

    static void dEnd();

    static void dSkip(int _bytes);

    static void dWord(std::vector<init_list_node>* _values);

};
