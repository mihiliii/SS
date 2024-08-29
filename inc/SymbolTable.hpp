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

    void write(std::ofstream* _file) override;

    Elf32_Sym* addSymbol(Elf32_Sym& _symbol_entry);

    Elf32_Sym* addSymbol(
        std::string _name, Elf32_Addr _value, bool _defined, Elf32_Half _section_index = -1, unsigned char _info = 0
    );

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

    void resolveLocalSymbol(Elf32_Sym* _symbol_entry, Elf32_Addr _address);

private:


    struct symbol_reference {
        bool pc_rel;               // If true, symbol will be put in literal pool and last 12b are replaced as offset.
        Elf32_Addr address;        // Address of the section that needs to be replaced with the symbol value.
        Elf32_Half section_index;  // Index of the section that needs to be replaced with the symbol value.
    };

    std::vector<Elf32_Sym*> content;
    std::map<std::string, std::list<symbol_reference>> symbol_bp_references;
};
