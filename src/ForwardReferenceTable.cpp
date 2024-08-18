#include "../inc/ForwardReferenceTable.hpp"

#include "../inc/Assembler.hpp"
#include "../inc/CustomSection.hpp"
#include "../inc/SectionHeaderTable.hpp"
#include "../inc/StringTable.hpp"
#include "../inc/SymbolTable.hpp"

void ForwardReferenceTable::addReference(Elf32_Sym* _symbol_entry, Elf32_Addr _address) {
    std::string symbol_name = Assembler::string_table->getString(_symbol_entry->st_name);
    if (forward_references.find(symbol_name) == forward_references.end()) {
        forward_references[symbol_name] = std::list<Elf32_Addr>();
    }
    forward_references[symbol_name].push_back(_address);
}

// Resolve symbol should be called after all sections are written to the file
// All symbols should be resolved
void ForwardReferenceTable::resolveSymbol(Elf32_Sym* _symbol_entry) {
    // std::string symbol_name = Assembler::string_table->getString(_symbol_entry->st_name);
    // std::string section_name =
    // Assembler::string_table->getString(Assembler::section_header_table->getSectionHeader(_symbol_entry->st_shndx)->sh_name);
    // if (forward_references.find(symbol_name) != forward_references.end()) {
    // for (Elf32_Addr& address : forward_references[symbol_name]) {
    // dynamic_cast<InputSection*>(Section::getSectionTable()[section_name])
    // ->overwriteContent(&_symbol_entry->st_value, sizeof(Elf32_Addr), address);
    // }
    // forward_references.erase(symbol_name);
    // }
}

void ForwardReferenceTable::resolveSymbol(std::string _symbol_name) {
    resolveSymbol(Assembler::symbol_table->findSymbol(_symbol_name));
}

void ForwardReferenceTable::print() const {
    for (const auto& entry : forward_references) {
        std::cout << entry.first << " -> ";
        for (const Elf32_Addr& address : entry.second) {
            std::cout << address << " ";
        }
        std::cout << std::endl;
    }
}

ForwardReferenceTable::ForwardReferenceTable() {}
