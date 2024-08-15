#include "../inc/ForwardReferenceTable.hpp"

#include "../inc/InputSection.hpp"
#include "../inc/SectionHeaderTable.hpp"
#include "../inc/StringTable.hpp"
#include "../inc/SymbolTable.hpp"

ForwardReferenceTable& ForwardReferenceTable::getInstance() {
    static ForwardReferenceTable instance;
    return instance;
}

void ForwardReferenceTable::addReference(Elf32_Sym* _symbol_entry, Elf32_Addr _address) {
    std::string symbol_name = StringTable::getInstance().getString(_symbol_entry->st_name);
    if (forward_references.find(symbol_name) == forward_references.end()) {
        forward_references[symbol_name] = std::list<Elf32_Addr>();
    }
    forward_references[symbol_name].push_back(_address);
}

void ForwardReferenceTable::resolveSymbol(Elf32_Sym* _symbol_entry) {
    StringTable& string_table = StringTable::getInstance();
    SectionHeaderTable& section_header_table = SectionHeaderTable::getInstance();
    std::string symbol_name = string_table.getString(_symbol_entry->st_name);
    std::string section_name =
        string_table.getString(section_header_table.getSectionHeader(_symbol_entry->st_shndx)->sh_name);
    if (forward_references.find(symbol_name) != forward_references.end()) {
        for (Elf32_Addr& address : forward_references[symbol_name]) {
            dynamic_cast<InputSection*>(Section::getSectionTable()[section_name])
                ->overwriteContent(&_symbol_entry->st_value, sizeof(Elf32_Addr), address);
        }
        forward_references.erase(symbol_name);
    }
}

void ForwardReferenceTable::resolveSymbol(std::string _symbol_name) {
    resolveSymbol(SymbolTable::getInstance().findSymbol(_symbol_name));
}

void ForwardReferenceTable::write(std::ofstream* file) {}

void ForwardReferenceTable::printContent() const {
    for (const auto& entry : forward_references) {
        std::cout << entry.first << " -> ";
        for (const Elf32_Addr& address : entry.second) {
            std::cout << address << " ";
        }
        std::cout << std::endl;
    }
}

ForwardReferenceTable::ForwardReferenceTable() : Section(std::string(".frtab")) {
    StringTable::getInstance().addString(std::string(".frtab"), &section_header.sh_name);
}
