#pragma once

#include <map>
#include <string>
#include <variant>
#include <vector>

#include "../Elf32/Elf32File.hpp"
#include "ConstantTable.hpp"
#include "ForwardReferenceTable.hpp"
#include "InstructionFormat.hpp"

typedef uint32_t Literal;
typedef std::string Symbol;

enum struct OPERAND_TYPE { LITERAL, SYMBOL };

struct Operand {
    OPERAND_TYPE type;
    std::variant<Literal, Symbol> value;

    Operand(Literal literal) : type(OPERAND_TYPE::LITERAL), value(literal) {}
    Operand(Symbol symbol) : type(OPERAND_TYPE::SYMBOL), value(symbol) {}

    bool is_literal() const
    {
        return type == OPERAND_TYPE::LITERAL;
    }
    bool is_symbol() const
    {
        return type == OPERAND_TYPE::SYMBOL;
    }

    Literal get_literal() const
    {
        return std::get<Literal>(value);
    }
    Symbol get_symbol() const
    {
        return std::get<Symbol>(value);
    }
};

class Assembler {
public:

    Assembler();

    int start_assembler(const std::string& input_file_name, const std::string& output_file_name);

    void section_dir(const std::string& section_name);

    void skip_dir(int bytes_to_skip);

    void word_dir(const std::vector<Operand>& values);

    void global_dir(const std::vector<Operand>& symbols);

    void extern_dir(const std::vector<Operand>& symbols);

    void define_label(const std::string& label);

    void halt();

    void interrupt();

    void iret();

    void exchange(REG reg_a, REG reg_b);

    void call(uint32_t literal);

    void call(const std::string& symbol);

    void jump(MOD mod, REG reg_a, REG reg_b, REG reg_c, uint32_t literal);

    void jump(MOD mod, REG reg_a, REG reg_b, REG reg_c, const std::string& symbol);

    void arithmetic_logic_shift(OC oc, MOD mod, REG src, REG dest);

    void arithmetic(MOD mod, REG src, REG dest);

    void logic(MOD mod, REG src, REG dest);

    void shift(MOD mod, REG src, REG dest);

    void push(REG reg);

    void pop(REG reg);

    void load(IF_ADDR addr, REG reg_a, REG reg_b, uint32_t literal);

    void load(IF_ADDR addr, REG reg_a, REG reg_b, const std::string& symbol);

    void csr_read(REG csr, REG gpr);

    void store(IF_ADDR addr, REG reg_a, REG reg_b, REG reg_c, uint32_t literal);

    void store(IF_ADDR addr, REG reg_a, REG reg_b, REG reg_c, const std::string& symbol);

    void csr_write(REG gpr, REG csr);

    friend class ForwardReferenceTable;
    friend class ConstantTable;

private:

    CustomSection* _current_section;
    ForwardReferenceTable _forward_reference_table;
    std::map<CustomSection*, ConstantTable> _constant_table_map;
    Elf32File _elf32_file;

}; // namespace Assembler
