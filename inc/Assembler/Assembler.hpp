#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "../Elf32File.hpp"
#include "../misc/Hardware.hpp"
#include "ForwardReferenceTable.hpp"

constexpr int OpModeShift = 24;
constexpr int RegAShift = 20;
constexpr int RegBShift = 16;
constexpr int RegCShift = 12;
constexpr int DispShift = 0;

constexpr uint32_t OpModeMask = 0xFF000000;
constexpr uint32_t RegAMask = 0x00F00000;
constexpr uint32_t RegBMask = 0x000F0000;
constexpr uint32_t RegCMask = 0x0000F000;
constexpr uint32_t DispMask = 0x00000FFF;

#define INSTRUCTION_SET_OP_MODE(instruction, op_mode) \
    (instruction & 0x00FFFFFF) | (((uint32_t) op_mode & 0xFF) << 24)
#define INSTRUCTION_SET_GPR_A(instruction, regA) \
    (instruction & 0xFF0FFFFF) | (((uint32_t) regA & 0xF) << 20)
#define INSTRUCTION_SET_GPR_B(instruction, regB) \
    (instruction & 0xFFF0FFFF) | (((uint32_t) regB & 0xF) << 16)
#define INSTRUCTION_SET_GPR_C(instruction, regC) \
    (instruction & 0xFFFF0FFF) | (((uint32_t) regC & 0xF) << 12)
#define INSTRUCTION_SET_DISP(instruction, disp) \
    (instruction & 0xFFFFF000) | ((uint32_t) disp & 0xFFF)

#define INSTRUCTION_CREATE(op_mode, regA, regB, regC, disp)                 \
    (((uint32_t) op_mode & 0xFF) << 24) | (((uint32_t) regA & 0xF) << 20) | \
        (((uint32_t) regB & 0xF) << 16) | (((uint32_t) regC & 0xF) << 12) | \
        ((uint32_t) disp & 0xFFF)

#define INSTRUCTION_GET_OP_MODE(instruction)        (instruction & 0xFF000000) >> 24
#define INSTRUCTION_GET_GPR_A(instruction)          (instruction & 0x00F00000) >> 20
#define INSTRUCTION_GET_GPR_B(instruction)          (instruction & 0x000F0000) >> 16
#define INSTRUCTION_GET_GPR_C(instruction)          (instruction & 0x0000F000) >> 12
#define INSTRUCTION_GET_DISP(instruction)           (instruction & 0x00000FFF)

#define INSTRUCTION_EXTRACT_BITS(instruction, mask) (instruction & mask)

class LiteralTable;

namespace Assembler {

struct Operand {
    std::string type;
    void* value;
};

enum struct AddrMode {
    IMMEDIATE,
    MEM_DIR,
    REG_DIR,
    REG_IND,
    REG_IND_OFF
};

int startAssembler(const char* _input_file_name, const char* _output_file_name);

namespace Instruction {

void halt();

void interrupt();

void iret();

void call(uint32_t value);

void call(const std::string& symbol);

void alu(OpMode op_mode, GPR source, GPR destination);

void jump(OpMode op_mode, GPR gprA, GPR gprB, GPR gprC, uint32_t value);

void jump(OpMode op_mode, GPR gprA, GPR gprB, GPR gprC, const std::string& symbol);

void push(GPR gpr);

void pop(GPR gpr);

void load(AddrMode addr, GPR gprA, GPR gprB, uint32_t value);

void load(AddrMode addr, GPR gprA, GPR gprB, const std::string& symbol);

void csr_read(CSR csr, GPR gpr);

void store(AddrMode addr_mode, GPR gprA, GPR gprB, GPR gprC, uint32_t value);

void store(AddrMode addr_mode, GPR gprA, GPR gprB, GPR gprC, const std::string& symbol);

void csr_write(GPR gpr, CSR csr);

void exchange(GPR gprB, GPR gprC);

};  // namespace Instruction

namespace Directive {

void section(const std::string& _section_name);

void skip(int _bytes);

void word(const std::vector<Operand>& _values);

void global_symbol(const std::vector<Operand>& _symbols);

void extern_symbol(const std::vector<Operand>& _symbols);

int label(const std::string& _label);

}  // namespace Directive

extern CustomSection* current_section;
extern Elf32File elf32_file;
extern ForwardReferenceTable forward_reference_table;
extern std::map<CustomSection*, LiteralTable> literal_table_map;

};  // namespace Assembler
