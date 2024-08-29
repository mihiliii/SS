#include "../inc/SymbolTable.hpp"

#include <iomanip>

#include "../inc/Assembler.hpp"
#include "../inc/Instructions.hpp"
#include "../inc/Section.hpp"
#include "../inc/StringTable.hpp"

SymbolTable::SymbolTable() : Section() {
    section_header.sh_name = Assembler::string_table->addString(".symtab");
    section_header.sh_type = SHT_SYMTAB;
    section_header.sh_entsize = sizeof(Elf32_Sym);
    section_header.sh_addralign = 4;
}

Elf32_Sym* SymbolTable::addSymbol(Elf32_Sym& _symbol_entry) {
    Elf32_Sym* symbol_entry = new Elf32_Sym(_symbol_entry);
    content.emplace_back(symbol_entry);
    section_header.sh_size += sizeof(Elf32_Sym);
    return symbol_entry;
}

Elf32_Sym* SymbolTable::addSymbol(
    std::string _name, Elf32_Addr _value, bool _defined, Elf32_Half _section_index, unsigned char _info
) {
    if (findSymbol(_name) != nullptr) {
        std::cerr << "Symbol " << _name << " already exists in the symbol table." << std::endl;
        return nullptr;
    }

    Elf32_Half section_index =
        ((short) _section_index == -1) ? Assembler::current_section->getSectionHeaderTableIndex() : _section_index;

    Elf32_Sym* symbol_entry = new Elf32_Sym(
        {.st_name = Assembler::string_table->addString(_name),
         .st_info = _info,
         .st_other = 0,
         .st_shndx = section_index,
         .st_value = _value,
         .st_size = 0,
         .st_defined = _defined}
    );
    content.emplace_back(symbol_entry);
    section_header.sh_size += sizeof(Elf32_Sym);
    return symbol_entry;
}

void SymbolTable::setInfo(std::string _name, Elf32_Half _info) {
    Elf32_Sym* symbol = findSymbol(_name);
    if (symbol != nullptr)
        setInfo(symbol, _info);
}

void SymbolTable::setInfo(Elf32_Sym* _symbol, Elf32_Half _info) {
    _symbol->st_info = _info;
}

Elf32_Sym* SymbolTable::findSymbol(std::string _name) {
    for (Elf32_Sym* symbol : content) {
        if (Assembler::string_table->getString(symbol->st_name) == _name)
            return symbol;
    }
    return nullptr;
}

void SymbolTable::defineSymbol(Elf32_Sym* _symbol_entry, Elf32_Addr _value) {
    _symbol_entry->st_value = _value;
    _symbol_entry->st_defined = true;
}

// Adds a symbol reference of the undefined symbol that will be resolved in backpatching phase.
// NOTE: This method needs to be called instead of addLiteralReference for undefined symbols.
void SymbolTable::addSymbolReference(Elf32_Sym* _symbol_entry, Elf32_Addr _address, bool _indirect) {
    std::string symbol_name = Assembler::string_table->getString(_symbol_entry->st_name);

    if (symbol_bp_references.find(symbol_name) == symbol_bp_references.end()) {
        symbol_bp_references[symbol_name] = std::list<symbol_reference>();
    }

    Elf32_Half current_section_index = Assembler::current_section->getSectionHeaderTableIndex();
    symbol_bp_references[symbol_name].push_back({_indirect, _address, current_section_index});
}

void SymbolTable::resolveLocalSymbol(Elf32_Sym* _symbol_entry, Elf32_Addr _address) {
    uint32_t offset = _symbol_entry->st_value - _address - sizeof(instruction_format);
    CustomSection* section = CustomSection::getSectionsMap()[Section::getName(_symbol_entry->st_shndx)];

    instruction_format old_instruction = *(instruction_format*) section->getContent(_address);
    instruction_format new_instruction;
    MOD_JMP mod;

    if (offset < 0xFFF) {
        OP_CODE op_code = (OP_CODE) ((old_instruction & 0xF0000000) >> 28);

        switch (op_code) {
            case OP_CODE::JMP: {
                mod = (MOD_JMP) INSTRUCTION_FORMAT_MOD(old_instruction);
                if ((uint8_t) mod >= 0x8)
                    mod = (MOD_JMP) ((uint8_t) mod - 0x8);
                new_instruction = (old_instruction & 0xF0FFF000) | (offset & 0xFFF) | ((uint32_t) mod << 24);
                break;
            }
            default:
            break;
        }
    }

    section->overwriteContent(&new_instruction, sizeof(instruction_format), _address);
}

void SymbolTable::resolveSymbolReferences() {
    for (auto& entry : symbol_bp_references) {
        std::string symbol_name = entry.first;
        Elf32_Sym* symbol_entry = findSymbol(symbol_name);

        CustomSection* resolving_section;

        if (symbol_entry->st_defined == false && ELF32_ST_BIND(symbol_entry->st_info) == STB_LOCAL) {
            std::cerr << "Symbol " << symbol_name << " is not defined." << std::endl;
            exit(-1);
        }

        for (symbol_reference& reference : entry.second) {
            resolving_section = CustomSection::getSectionsMap()[Section::getName(reference.section_index)];

            if (symbol_entry->st_defined == false && ELF32_ST_BIND(symbol_entry->st_info) == STB_GLOBAL) {
                resolving_section->getLiteralTable().addUndefinedSymbolReference(symbol_entry, reference.address);
            }
            if (symbol_entry->st_defined == true && reference.pc_rel == false) {
                resolving_section->overwriteContent(&symbol_entry->st_value, sizeof(Elf32_Addr), reference.address);
            }
            else if (symbol_entry->st_defined == true && reference.section_index == symbol_entry->st_shndx) {
                resolveLocalSymbol(symbol_entry, reference.address);
            }
            else if (symbol_entry->st_defined == true && reference.pc_rel == true) {
                resolving_section->getLiteralTable().addLiteralReference(symbol_entry->st_value, reference.address);
            }
        }
    }
}

void SymbolTable::print() const {
    std::cout << "Symbol Table:" << std::endl;
    std::cout << "  ";
    std::cout << std::left << std::setfill(' ');
    std::cout << std::setw(4) << "NUM";
    std::cout << std::setw(25) << "NAME";
    std::cout << std::setw(9) << "VALUE";
    std::cout << std::setw(9) << "SIZE";
    std::cout << std::setw(5) << "INFO";
    std::cout << std::setw(6) << "OTHER";
    std::cout << std::setw(8) << "SHINDEX";
    std::cout << "DEFINED";
    std::cout << std::endl;
    uint32_t i = 0;
    for (Elf32_Sym* c : content) {
        std::cout << "  ";
        std::cout << std::right << std::setfill(' ') << std::dec;
        std::cout << std::setw(3) << i << " ";
        std::cout << std::left;
        std::cout << std::setw(24) << Assembler::string_table->getString(c->st_name) << " ";
        std::cout << std::right << std::setfill('0') << std::hex;
        std::cout << std::setw(8) << c->st_value << " ";
        std::cout << std::setw(8) << c->st_size << " ";
        std::cout << std::setw(4) << (int) c->st_info << " ";
        std::cout << std::setw(5) << (int) c->st_other << " ";
        std::cout << std::setfill(' ') << std::dec << std::left;
        std::cout << std::setw(7) << c->st_shndx << " ";
        std::cout << (c->st_defined ? "true" : "false");
        std::cout << std::endl;
        i += 1;
    }
}

void SymbolTable::write(std::ofstream* _file) {
    section_header.sh_size = content.size() * sizeof(Elf32_Sym);

    if (_file->tellp() % section_header.sh_addralign != 0) {
        _file->write("\0", section_header.sh_addralign - (_file->tellp() % section_header.sh_addralign));
    }

    section_header.sh_offset = _file->tellp();

    for (Elf32_Sym* symbol_entry : content) {
        _file->write((char*) symbol_entry, sizeof(Elf32_Sym));
    }
}
