#include "../inc/ForwardReferenceTable.hpp"

#include "../inc/CustomSection.hpp"
#include "../inc/Assembler.hpp"
#include "../inc/Instructions.hpp"

// Adds a symbol reference of the symbol that will be resolved in backpatching phase.
void ForwardReferenceTable::add(Elf32_Sym* _symbol_entry, Elf32_Addr _address, bool _direct_ref) {
    std::string symbol_name = Assembler::string_table->getString(_symbol_entry->st_name);

    if (forward_references.find(symbol_name) == forward_references.end()) {
        forward_references[symbol_name] = std::list<symbol_reference>();
    }

    Elf32_Half current_section_index = Assembler::current_section->getSectionHeaderTableIndex();
    forward_references[symbol_name].push_back({_direct_ref, _address, current_section_index});
}

void ForwardReferenceTable::resolveSymbolForwardReferences() {
    // Check if all symbols are defined.
    for (auto& symbol_entry : Assembler::symbol_table->content) {
        if (symbol_entry->st_defined == false && ELF32_ST_BIND(symbol_entry->st_info) == STB_LOCAL) {
            std::cerr << "Symbol " << StringTable::getName(symbol_entry->st_name) << " is not defined." << std::endl;
            exit(-1);
        }
    }

    for (auto& entry : forward_references) {
        std::string symbol_name = entry.first;
        Elf32_Sym* symbol_entry = Assembler::symbol_table->getSymbol(symbol_name);

        CustomSection* resolving_section;

        if (symbol_entry->st_defined == true) {
            for (symbol_reference& reference : entry.second) resolveSymbol(symbol_entry, reference);
        }
        else if (symbol_entry->st_defined == false && ELF32_ST_BIND(symbol_entry->st_info) == STB_GLOBAL) {
            for (symbol_reference& reference : entry.second) {
                resolving_section = CustomSection::getSectionsMap()[Section::getName(reference.section_index)];
                resolving_section->getLiteralTable().addRelocatableSymbolReference(symbol_entry, reference.address);
            }
        }
        else {
            std::cerr << "UNEXPECTED ERROR AT RESOLVE SYMBOL FORWARD REFERENCES" << std::endl;
            exit(-1);
        }
    }
}

void ForwardReferenceTable::resolveSymbol(Elf32_Sym* _symbol_entry, symbol_reference& _reference) {
    CustomSection* section = CustomSection::getSectionsMap()[Section::getName(_reference.section_index)];

    // If indirect_ref is false then the symbol value needs to be placed in the appropriate section on address
    // _reference.address.
    if (_reference.direct_ref == true) {
        section->overwriteContent(&_symbol_entry->st_value, sizeof(Elf32_Addr), _reference.address);
        return;
    }
    else if (_reference.section_index != _symbol_entry->st_shndx) {
        section->getLiteralTable().addRelocatableSymbolReference(_symbol_entry, _reference.address);
        return;
    }

    // For branch instructions, if the symbol (label) is in the same section as branch instruction and displacement
    // is smaller than 12 bits then branch instruction MOD can be changed to direct addressing mode and displacement
    // can be replaced with the offset to that symbol value (address).
    uint32_t offset = _symbol_entry->st_value - _reference.address - sizeof(instruction_format);

    instruction_format old_instruction = *(instruction_format*) section->getContent(_reference.address);
    instruction_format new_instruction;

    if (offset < 0xFFF) {
        OP_CODE op_code = (OP_CODE) ((old_instruction & 0xF0000000) >> 28);

        switch (op_code) {
            case OP_CODE::JMP: {
                uint8_t mod = INSTRUCTION_FORMAT_MOD(old_instruction);
                mod = (mod >= 0x8) ? (mod - 0x8) : mod;
                new_instruction = (old_instruction & 0xF0FFF000) | (offset & 0xFFF) | ((uint32_t) mod << 24);
                break;
            }
            default:
                break;
        }
    }

    section->overwriteContent(&new_instruction, sizeof(instruction_format), _reference.address);
}