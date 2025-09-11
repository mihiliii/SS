#include "../../inc/Assembler/Assembler.hpp"

#include <cstdint>
#include <iostream>

#include "../../inc/Elf32/Elf32File.hpp"
#include "Assembler/ConstantTable.hpp"
#include "Assembler/InstructionFormat.hpp"
#include "Elf32/CustomSection.hpp"
#include "Elf32/Elf32.hpp"

// Include the Flex and Bison headers to use their functions:
extern int yylex();
extern int yyparse();
extern FILE* yyin;

extern uint32_t line; // Current line number in the source file

Assembler::Assembler()
    : _current_section(nullptr),
      _forward_reference_table(ForwardReferenceTable(*this)),
      _constant_table_map(),
      _elf32_file(Elf32File())
{
}

int Assembler::start_assembler(const std::string& input_file_name,
                               const std::string& output_file_name)
{
    // Open a file handle to a particular file:
    FILE* f_input = fopen(input_file_name.c_str(), "r");

    // Make sure it is valid:
    if (!f_input) {
        std::cerr << "Error: can't open file " << input_file_name << std::endl;
        return -1;
    }

    // Set Flex to read from it instead of defaulting to STDIN:
    yyin = f_input;

    // Parse through the input, if there is an error, yyparse will return a non-zero value:
    if (yyparse()) {
        std::cerr << "Error: parsing failed." << std::endl;
        return -1;
    }

    // Close the file handle:
    fclose(f_input);

    // Backpatching phase:
    _forward_reference_table.backpatch();
    for (auto& iterator : _constant_table_map) {
        ConstantTable& constant_table = iterator.second;
        constant_table.resolve_references();
        constant_table.add_literal_pool_to_section();
    }

    // Write the ELF file:
    _elf32_file.write_bin(output_file_name, ET_REL);
    _elf32_file.read_elf(output_file_name);

    return 0;
}

void Assembler::section_dir(const std::string& section_name)
{
    auto it = _elf32_file.custom_section_map.find(section_name);

    if (it != _elf32_file.custom_section_map.end()) {
        CustomSection* section = &it->second;
        _current_section = section;
    }
    else {
        _current_section = _elf32_file.new_custom_section(section_name);
        _elf32_file.symbol_table.add_symbol(_current_section->get_name(), 0, true,
                                            _current_section->get_header_index(),
                                            ELF32_ST_INFO(STB_LOCAL, STT_SECTION));
        _constant_table_map.try_emplace(_current_section,
                                        ConstantTable(_elf32_file, *_current_section));
    }
}

void Assembler::skip_dir(int bytes_to_skip)
{
    std::vector<char> vector(bytes_to_skip, 0);
    _current_section->append_data(vector.data(), bytes_to_skip);
}

void Assembler::word_dir(const std::vector<Operand>& values)
{
    for (const Operand& node : values) {

        if (node.type == OPERAND_TYPE::LITERAL) {
            _current_section->append_data(std::get<Literal>(node.value));
        }

        else if (node.type == OPERAND_TYPE::SYMBOL) {
            const std::string symbol_name = std::get<std::string>(node.value);
            Elf32_Sym* symbol_entry = _elf32_file.symbol_table.find_symbol(symbol_name);

            if (symbol_entry == nullptr) {
                symbol_entry = &_elf32_file.symbol_table.add_symbol(
                    symbol_name, 0, false, _current_section->get_header_index());
            }

            Elf32_Word symbol_index = _elf32_file.symbol_table.get_symbol_index(*symbol_entry);

            _current_section->get_rela_table().add_entry(
                _current_section->get_size(), ELF32_R_INFO(ELF32_R_TYPE_ABS32, symbol_index), 0);

            _current_section->append_data((instruction_format_t) 0);
        }
    }
}

void Assembler::global_dir(const std::vector<Operand>& symbols)
{
    // TODO: check if there is a global symbol that is undefined
    for (const Operand& node : symbols) {
        const std::string symbol_name = std::get<std::string>(node.value);
        Elf32_Sym* symbol_entry = _elf32_file.symbol_table.find_symbol(symbol_name);

        if (symbol_entry == nullptr) {
            _elf32_file.symbol_table.add_symbol(symbol_name, 0, false, SHN_ABS,
                                                ELF32_ST_INFO(STB_GLOBAL, STT_NOTYPE));
        }
        else {
            symbol_entry->st_info = ELF32_ST_INFO(STB_GLOBAL, ELF32_ST_TYPE(symbol_entry->st_info));
        }
    }
}

void Assembler::extern_dir(const std::vector<Operand>& symbols)
{
    for (const Operand& node : symbols) {
        const std::string symbol_name = std::get<std::string>(node.value);
        Elf32_Sym* symbol_entry = _elf32_file.symbol_table.find_symbol(symbol_name);

        if (symbol_entry == nullptr) {
            _elf32_file.symbol_table.add_symbol(symbol_name, 0, false, SHN_ABS,
                                                ELF32_ST_INFO(STB_GLOBAL, STT_NOTYPE));
        }
        else {
            symbol_entry->st_info = ELF32_ST_INFO(STB_GLOBAL, ELF32_ST_TYPE(symbol_entry->st_info));
        }
    }
}

void Assembler::define_label(const std::string& label)
{
    Elf32_Off location_counter = _current_section->get_size();
    Elf32_Sym* symbol_entry = _elf32_file.symbol_table.find_symbol(label);

    if (symbol_entry != nullptr) {
        if (symbol_entry->st_defined == true) {
            std::cerr << "Symbol " << label << " already defined!" << std::endl;
            exit(-1);
        }
        else {
            _elf32_file.symbol_table.define_symbol(*symbol_entry, location_counter,
                                                   _current_section->get_header_index());
        }
    }
    else {
        _elf32_file.symbol_table.add_symbol(label, location_counter, true,
                                            _current_section->get_header_index());
    }
}

void Assembler::halt()
{
    _current_section->append_data(if_create(OC::HALT, MOD::HALT, REG::R0, REG::R0, REG::R0, 0));
}

void Assembler::interrupt()
{
    _current_section->append_data(if_create(OC::INT, MOD::INT, REG::R0, REG::R0, REG::R0, 0));
}

void Assembler::iret()
{
    _current_section->append_data(
        if_create(OC::LD, MOD::LD_CSR_REGIND_INC, REG::STATUS, REG::SP, REG::R0, SP_INC));

    _current_section->append_data(
        if_create(OC::LD, MOD::LD_GPR_REGIND_INC, REG::PC, REG::SP, REG::R0, SP_INC));
}

void Assembler::call(uint32_t literal)
{
    instruction_format instruction;

    if (literal <= MAX_DISP) {
        instruction = if_create(OC::CALL, MOD::CALL, REG::R0, REG::R0, REG::R0, literal);
    }
    else {
        ConstantTable& constant_table = _constant_table_map.at(_current_section);
        constant_table.add_literal_reference(literal, _current_section->get_size());

        instruction = if_create(OC::CALL, MOD::CALL_IND, REG::PC, REG::R0, REG::R0, 0);
    }

    _current_section->append_data(instruction);
}

void Assembler::call(const std::string& symbol)
{
    Elf32_Sym* symbol_entry = _elf32_file.symbol_table.find_symbol(symbol);

    if (symbol_entry == nullptr) {
        symbol_entry = &_elf32_file.symbol_table.add_symbol(symbol, 0, false,
                                                            _current_section->get_header_index());
    }

    _forward_reference_table.add_reference(*symbol_entry, _current_section->get_size());
    _current_section->append_data(if_create(OC::CALL, MOD::CALL_IND, REG::PC, REG::R0, REG::R0, 0));
}

void Assembler::arithmetic(MOD mod, REG src, REG dest)
{
    _current_section->append_data(if_create(OC::AR, mod, dest, dest, src, 0));
}

void Assembler::logic(MOD mod, REG src, REG dest)
{
    _current_section->append_data(if_create(OC::LOG, mod, dest, dest, src, 0));
}

void Assembler::shift(MOD mod, REG src, REG dest)
{
    _current_section->append_data(if_create(OC::SHF, mod, dest, dest, src, 0));
}

void Assembler::jump(MOD mod, REG reg_a, REG reg_b, REG reg_c, uint32_t literal)
{
    instruction_format instruction;
    const uint32_t JMP_IND_MOD_BIT = 0x8;

    if (literal > MAX_DISP) {

        if ((uint8_t) mod < JMP_IND_MOD_BIT) {
            mod = (MOD) ((uint8_t) mod + JMP_IND_MOD_BIT);
        }

        ConstantTable& constant_table = _constant_table_map.at(_current_section);
        constant_table.add_literal_reference(literal, _current_section->get_size());

        instruction = if_create(OC::JMP, mod, REG::R0, reg_b, reg_c, 0);
    }
    else {
        instruction = if_create(OC::JMP, mod, REG::R0, reg_b, reg_c, literal);
    }

    _current_section->append_data(instruction);
}

void Assembler::jump(MOD mod, REG reg_a, REG reg_b, REG reg_c, const std::string& symbol)
{
    const uint32_t JMP_IND_MOD_BIT = 0x8;

    if ((uint8_t) mod < JMP_IND_MOD_BIT) {
        mod = (MOD) ((uint32_t) mod + JMP_IND_MOD_BIT);
    }

    Elf32_Sym* symbol_entry = _elf32_file.symbol_table.find_symbol(symbol);
    if (symbol_entry == nullptr) {
        symbol_entry = &_elf32_file.symbol_table.add_symbol(symbol, 0, false,
                                                            _current_section->get_header_index());
    }

    _forward_reference_table.add_reference(*symbol_entry, _current_section->get_size());
    instruction_format instruction = if_create(OC::JMP, mod, REG::PC, reg_b, reg_c, 0);

    _current_section->append_data(instruction);
}

void Assembler::push(REG reg)
{
    _current_section->append_data(
        if_create(OC::ST, MOD::ST_INC_REGIND, REG::SP, REG::R0, reg, -SP_INC));
}

void Assembler::pop(REG reg)
{
    _current_section->append_data(
        if_create(OC::LD, MOD::LD_GPR_REGIND_INC, reg, REG::SP, REG::R0, SP_INC));
}

void Assembler::load(IF_ADDR addr, REG reg_a, REG reg_b, uint32_t literal)
{
    instruction_format instruction;

    switch (addr) {
    case IF_ADDR::IMMEDIATE: {
        if (literal <= MAX_DISP) {
            instruction = if_create(OC::LD, MOD::LD_GPR_GPR_DSP, reg_a, REG::R0, REG::R0, literal);
        }
        else {
            ConstantTable& constant_table = _constant_table_map.at(_current_section);
            constant_table.add_literal_reference(literal, _current_section->get_size());

            instruction = if_create(OC::LD, MOD::LD_GPR_REGIND_DSP, reg_a, REG::PC, REG::R0, 0);
        }
        break;
    }
    case IF_ADDR::MEM_DIR: {
        if (literal <= MAX_DISP) {
            instruction =
                if_create(OC::LD, MOD::LD_GPR_REGIND_DSP, reg_a, REG::R0, REG::R0, literal);
        }
        else {
            ConstantTable& constant_table = _constant_table_map.at(_current_section);
            constant_table.add_literal_reference(literal, _current_section->get_size());

            instruction = if_create(OC::LD, MOD::LD_GPR_REGIND_DSP, reg_a, REG::PC, REG::R0, 0);
            _current_section->append_data(instruction);

            instruction = if_create(OC::LD, MOD::LD_GPR_REGIND_DSP, reg_a, reg_a, REG::R0, 0);
        }
        break;
    }
    case IF_ADDR::REG_DIR: {
        instruction = if_create(OC::LD, MOD::LD_GPR_GPR_DSP, reg_a, reg_b, REG::R0, 0);
        break;
    }
    case IF_ADDR::REG_IND: {
        instruction = if_create(OC::LD, MOD::LD_GPR_REGIND_DSP, reg_a, reg_a, REG::R0, 0);
        break;
    }
    case IF_ADDR::REG_IND_OFF: {
        if (literal > MAX_DISP) {
            // TODO: change std::cout to exception
            std::cout << std::dec << "Error at line " << line << ": ";
            std::cout << "offset is too large" << std::endl;
            exit(-1);
        }
        instruction = if_create(OC::LD, MOD::LD_GPR_REGIND_DSP, reg_a, reg_b, REG::R0, literal);
        break;
    }
    default:
        std::cout << "Error at line " << line << ": ";
        std::cout << "unknown addressing mode in load instruction" << std::endl;
        exit(-1);
        break;
    }

    _current_section->append_data(instruction);
}

void Assembler::load(IF_ADDR addr, REG reg_a, REG reg_b, const std::string& symbol)
{
    instruction_format instruction;

    Elf32_Sym* symbol_entry = _elf32_file.symbol_table.find_symbol(symbol);

    if (symbol_entry == nullptr) {
        symbol_entry = &_elf32_file.symbol_table.add_symbol(symbol, 0, false,
                                                            _current_section->get_header_index());
    }

    switch (addr) {
    case IF_ADDR::IMMEDIATE: {
        _forward_reference_table.add_reference(*symbol_entry, _current_section->get_size());
        instruction = if_create(OC::LD, MOD::LD_GPR_REGIND_DSP, reg_a, REG::PC, REG::R0, 0);
        break;
    }
    case IF_ADDR::MEM_DIR: {
        _forward_reference_table.add_reference(*symbol_entry, _current_section->get_size());
        instruction = if_create(OC::LD, MOD::LD_GPR_REGIND_DSP, reg_a, REG::PC, REG::R0, 0);

        _current_section->append_data(instruction);

        instruction = if_create(OC::LD, MOD::LD_GPR_REGIND_DSP, reg_a, reg_a, REG::R0, 0);
        break;
    }
    default:
        std::cout << "Error at line " << line << ": ";
        std::cout << "unknown addressing mode in load instruction" << std::endl;
        exit(-1);
        break;
    }

    _current_section->append_data(instruction);
}

void Assembler::csr_read(REG csr, REG gpr)
{
    _current_section->append_data(if_create(OC::LD, MOD::LD_GPR_CSR, gpr, csr, REG::R0, 0));
}

void Assembler::store(IF_ADDR addr, REG reg_a, REG reg_b, REG reg_c, uint32_t literal)
{
    instruction_format instruction;

    switch (addr) {
    case IF_ADDR::IMMEDIATE: {
        // TODO: change std::cout to exception
        std::cout << "Error at line " << line << ": ";
        std::cout << "immediate addressing usage in store instruction" << std::endl;
        exit(-1);
        break;
    }
    case IF_ADDR::MEM_DIR: {
        if (literal <= MAX_DISP) {
            instruction = if_create(OC::ST, MOD::ST_REGIND, REG::R0, REG::R0, reg_c, literal);
        }
        else {
            ConstantTable& constant_table = _constant_table_map.at(_current_section);
            constant_table.add_literal_reference(literal, _current_section->get_size());

            instruction = if_create(OC::ST, MOD::ST_MEMIND_REGIND, REG::PC, REG::R0, reg_c, 0);
        }
        break;
    }
    case IF_ADDR::REG_DIR: {
        instruction = if_create(OC::LD, MOD::LD_GPR_GPR_DSP, reg_a, reg_b, REG::R0, 0);
        break;
    }
    case IF_ADDR::REG_IND: {
        instruction = if_create(OC::ST, MOD::ST_REGIND, reg_a, REG::R0, reg_c, 0);
        break;
    }
    case IF_ADDR::REG_IND_OFF: {
        if (literal > MAX_DISP) {
            std::cout << std::dec << "Error at line " << line << ": ";
            std::cout << "offset is too large" << std::endl;
            exit(-1);
        }
        instruction = if_create(OC::ST, MOD::ST_REGIND, reg_a, REG::R0, reg_c, literal);
        break;
    }
    default:
        std::cout << "Error at line " << line << ": ";
        std::cout << "unknown addressing mode in store instruction" << std::endl;
        exit(-1);
        break;
    }

    _current_section->append_data(instruction);
}

void Assembler::store(IF_ADDR addr, REG reg_a, REG reg_b, REG reg_c, const std::string& symbol)
{
    instruction_format instruction;

    Elf32_Sym* symbol_entry = _elf32_file.symbol_table.find_symbol(symbol);

    if (symbol_entry == nullptr) {
        symbol_entry = &_elf32_file.symbol_table.add_symbol(symbol, 0, false,
                                                            _current_section->get_header_index());
    }

    switch (addr) {
    case IF_ADDR::IMMEDIATE: {
        std::cout << std::dec << "Error at line " << line << ": ";
        std::cout << "immediate addressing usage in store instruction" << std::endl;
        exit(-1);
        break;
    }
    case IF_ADDR::MEM_DIR: {
        _forward_reference_table.add_reference(*symbol_entry, _current_section->get_size());
        instruction = if_create(OC::ST, MOD::ST_MEMIND_REGIND, REG::PC, REG::R0, reg_c, 0);
        break;
    }
    default:
        std::cout << "Error at line " << line << ": ";
        std::cout << "unknown addressing mode in store instruction" << std::endl;
        exit(-1);
        break;
    }

    _current_section->append_data(instruction);
}

void Assembler::csr_write(REG gpr, REG csr)
{
    _current_section->append_data(if_create(OC::LD, MOD::LD_CSR_GPR, csr, gpr, REG::R0, 0));
}

void Assembler::exchange(REG source, REG dest)
{
    _current_section->append_data(if_create(OC::XCHG, MOD::XCHG, REG::R0, dest, source, 0));
}
