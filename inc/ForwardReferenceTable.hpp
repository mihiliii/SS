#pragma once

#include "Section.hpp"
#include "Elf32.hpp"
#include <utility>
#include <list>
#include <map>

class ForwardReferenceTable : Section {
public:

    ForwardReferenceTable& getInstance();

    void addReference(Elf32_Sym* _symbol_entry, Elf32_Addr _address);

    void write(std::ofstream* file) override;

    void printContent() const override;

    ForwardReferenceTable(const ForwardReferenceTable&) = delete;
    ForwardReferenceTable& operator=(const ForwardReferenceTable&) = delete;

    ~ForwardReferenceTable() = default;

private:

    ForwardReferenceTable();

    std::map<std::string, std::list<Elf32_Fr>> forward_references; 


};