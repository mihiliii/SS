#include "../../inc/Assembler/ForwardReferenceTable.hpp"
#include "../../inc/Assembler/Assembler.hpp"
#include "../../inc/Assembler/InstructionFormat.hpp"

ForwardReferenceTable::ForwardReferenceTable(Assembler& assembler)
    : _assembler(assembler),
      _elf32_file(assembler._elf32_file),
      forward_references()
{
}

// Adds a symbol reference of the symbol that will be resolved in backpatching phase.
void ForwardReferenceTable::add_reference(Elf32_Sym& symbol_entry, Elf32_Addr address)
{
    Elf32_Sym* symbol_entry_ptr = &symbol_entry;

    if (forward_references.find(symbol_entry_ptr) == forward_references.end()) {
        forward_references[symbol_entry_ptr] = std::list<SymbolReference>();
    }

    forward_references[symbol_entry_ptr].push_back({address, _assembler._current_section});
}

void ForwardReferenceTable::backpatch()
{
    for (const auto& entry : forward_references) {
        Elf32_Sym& symbol_entry = *entry.first;

        // Check if symbol is not defined and local since every symbol should be defined at the
        // backpatching phase.
        if (symbol_entry.st_defined == false && ELF32_ST_BIND(symbol_entry.st_info) == STB_LOCAL) {
            std::cerr << "Symbol " << _elf32_file.string_table.get_string(symbol_entry.st_name)
                      << " is not defined." << std::endl;
            exit(-1);
        }

        for (SymbolReference reference : entry.second) {
            resolve_symbol(symbol_entry, reference);
        }
    }
}

void ForwardReferenceTable::resolve_symbol(Elf32_Sym& symbol_entry, SymbolReference& reference)
{
    instruction_format instruction = reference.section->get_data()[reference.address];
    uint32_t offset = symbol_entry.st_value - reference.address - sizeof(instruction);
    OC oc = if_get_oc(instruction);

    // For branch instructions jump location can be changed to label directly depending if that
    // label definition is in the same section as the branch instruction. Other instructions have
    // to access symbol value using PC relative addressing mode, where the symbol value is located
    // in the constant pool.
    if (oc == OC::JMP && offset < MAX_DISP) {
        MOD mod = if_get_mod(instruction);
        if (mod >= MOD::JMP_IND) {
            switch (mod) {
            case MOD::JMP_IND:
                mod = MOD::JMP;
                break;
            case MOD::BEQ_IND:
                mod = MOD::BEQ;
                break;
            case MOD::BNE_IND:
                mod = MOD::BNE;
                break;
            case MOD::BGT_IND:
                mod = MOD::BGT;
                break;
            default:
                std::cerr << "Error: something is not right.";
                exit(-1);
                break;
            }
        }

        const REG& reg_b = if_get_reg_b(instruction);
        const REG& reg_c = if_get_reg_c(instruction);

        instruction = if_create(oc, mod, REG::PC, reg_b, reg_c, offset);

        reference.section->overwrite_data(&instruction, sizeof(instruction), reference.address);
    }
    else {
        ConstantTable& constant_table = _assembler._constant_table_map.at(reference.section);
        constant_table.add_symbol_reference(symbol_entry, reference.address);
    }
}
