#include "../inc/Assembler/ForwardReferenceTable.hpp"

#include "../inc/Assembler/Assembler.hpp"
#include "../inc/Assembler/CustomSection.hpp"
#include "../inc/Assembler/Instructions.hpp"

// Adds a symbol reference of the symbol that will be resolved in backpatching phase.
void ForwardReferenceTable::add(Elf32_Sym* _symbol_entry, Elf32_Addr _address) {
    std::string symbol_name = Assembler::string_table->getString(_symbol_entry->st_name);

    if (forward_references.find(symbol_name) == forward_references.end()) {
        forward_references[symbol_name] = std::list<symbol_reference>();
    }

    Elf32_Half current_section_index = Assembler::current_section->getSectionHeaderTableIndex();
    forward_references[symbol_name].push_back({_address, current_section_index});
}

void ForwardReferenceTable::backpatch() {
    for (auto& entry : forward_references) {
        std::string symbol_name = entry.first;
        Elf32_Sym* symbol_entry = Assembler::symbol_table->getSymbol(symbol_name);

        // Check if symbol is defined and local.
        if (symbol_entry->st_defined == false && ELF32_ST_BIND(symbol_entry->st_info) == STB_LOCAL) {
            std::cerr << "Symbol " << StringTable::getName(symbol_entry->st_name) << " is not defined." << std::endl;
            exit(-1);
        }

        for (symbol_reference& reference : entry.second) resolveSymbol(symbol_entry, reference);
    }
}

void ForwardReferenceTable::resolveSymbol(Elf32_Sym* _symbol_entry, symbol_reference& _reference) {
    CustomSection* section = CustomSection::getSectionsMap()[Section::getName(_reference.section_index)];

    instruction_format instruction = *(instruction_format*) section->getContent(_reference.address);
    OP_CODE op_code = (OP_CODE) INSTRUCTION_FORMAT_OP_CODE(instruction);
    uint32_t offset = _symbol_entry->st_value - _reference.address - sizeof(instruction_format);

    // For branch instructions jump location can be changed to an symbol directly depending if that symbol definition is
    // in the same section as the branch instruction. Other instructions have to access symbol value using PC relative
    // addressing mode, where the symbol value is located in the literal table.
    if (op_code == OP_CODE::JMP && offset < 0xFFF) {
        uint8_t mod = INSTRUCTION_FORMAT_MOD(instruction);
        mod = (mod >= 0x8) ? (mod - 0x8) : mod;
        uint32_t reg_A = 0;
        instruction = (instruction & 0xF00FF000) | ((uint32_t) mod << 24) | (reg_A << 20) | (offset & 0xFFF);

        section->overwriteContent(&instruction, sizeof(instruction_format), _reference.address);
    }
    else {
        section->getLiteralTable().addRelocatableSymbolReference(_symbol_entry, _reference.address);
    }
}