#include "../inc/ForwardReferenceTable.hpp"

#include "../inc/StringTable.hpp"

ForwardReferenceTable& ForwardReferenceTable::getInstance() {
    static ForwardReferenceTable instance;
    return instance;
}

void ForwardReferenceTable::addReference(Elf32_Sym* _symbol_entry, Elf32_Addr _address) {
    Elf32_Fr fr = {};
    fr.fr_addr = _address;
    fr.fr_next = nullptr;
    std::string symbol_name = StringTable::getInstance().getString(_symbol_entry->st_name);
    if (forward_references.find(symbol_name) == forward_references.end()) {
        forward_references[symbol_name] = std::list<Elf32_Fr>();
    }
    forward_references[symbol_name].push_back(fr);
}

void ForwardReferenceTable::write(std::ofstream* file) {}

void ForwardReferenceTable::printContent() const {}

ForwardReferenceTable::ForwardReferenceTable() : Section(std::string(".frtab")) {}
