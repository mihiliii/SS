#pragma once

#include <iostream>
#include <map>
#include <list>
#include <vector>

#include "Elf32.hpp"
#include "Section.hpp"

class SymbolTable : public Section {
public:

    SymbolTable();

    void write(std::ofstream* _file) override;

    Elf32_Sym* addSymbol(Elf32_Sym& _symbol_entry);

    Elf32_Sym* addSymbol(std::string _name, Elf32_Addr _value, bool _defined);

    void setInfo(std::string _name, Elf32_Half _info);

    void setInfo(Elf32_Sym* _symbol, Elf32_Half _info);

    Elf32_Sym* findSymbol(std::string _name);

    void defineSymbol(Elf32_Sym* _symbol_entry, Elf32_Addr _value);

    void addSymbolReference(Elf32_Sym* _symbol_entry, Elf32_Addr _address, bool _indirect = false);
    
    void resolveSymbolReferences();

    void print() const;

    SymbolTable(const SymbolTable&) = delete;
    SymbolTable& operator=(const SymbolTable&) = delete;

    ~SymbolTable() = default;

private:

    struct symbol_reference {
        bool indirect;
        Elf32_Addr address;
    };

    std::vector<Elf32_Sym*> content;
    std::map<std::string, std::list<symbol_reference>> symbol_bp_references;
};
