#pragma once

#include <iostream>
#include <list>
#include <map>
#include <vector>

#include "Elf32.hpp"
#include "Section.hpp"


class SymbolTable : public Section {
public:

    SymbolTable();
    
    friend class ForwardReferenceTable;

    void write(std::ofstream* _file) override;

    Elf32_Sym* addSymbol(Elf32_Sym& _symbol_entry);

    Elf32_Sym* addSymbol(
        std::string _name, Elf32_Addr _value, bool _defined, Elf32_Half _section_index = -1, unsigned char _info = 0
    );

    Elf32_Sym* getSymbol(std::string _name);

    Elf32_Sym* getSymbol(uint32_t _entry_index);

    uint32_t getSymbolEntryIndex(std::string _name);

    uint32_t getSymbolEntryIndex(Elf32_Sym* _symbol_entry);

    void setInfo(std::string _name, Elf32_Half _info);

    void setInfo(Elf32_Sym* _symbol, Elf32_Half _info);

    void defineSymbol(Elf32_Sym* _symbol_entry, Elf32_Addr _value);

    void print() const;

    SymbolTable(const SymbolTable&) = delete;
    SymbolTable& operator=(const SymbolTable&) = delete;

    ~SymbolTable() = default;

private:


    std::vector<Elf32_Sym*> content;
};
