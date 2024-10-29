%{
    #include <cstdio>
    #include <iostream>
    #include <cmath>
    #include "../inc/Assembler/Assembler.hpp"
    #include "../inc/Assembler/Instructions.hpp"
    #include "../inc/Assembler/Directives.hpp"
    #include "../inc/SymbolTable.hpp"

    using namespace std;

    extern int yylex();
    extern int yyparse();
    extern FILE *yyin;
 
    void yyerror(const char *s);

    extern uint32_t line;
%}

%code requires {
    #include <vector>
    #include <cstdint>
    struct Operand;
}

// Bison fundamentally works by asking flex to get the next token, which it
// returns as an object of type "yystype". Initially (by default), yystype
// is merely a typedef of "int", but for non-trivial projects, tokens could
// be of any arbitrary data type. So, to deal with that, the idea is to
// override yystype's default typedef to be a C union instead. Unions can
// hold all of the types of tokens that Flex could return, and this this means
// we can return ints or floats or strings cleanly. Bison implements this
// mechanism with the %union directive:
%union {
    uint32_t uint_val;
    char* s_val;
    std::vector<Operand> *init_list;
}

// Define the "terminal symbol" token types I'm going to use (in CAPS
// by convention), and associate each with a field of the %union:
%token <uint_val> LITERAL
%token <s_val> SYMBOL
%token <uint_val> GPR
%token <uint_val> CSR

%token HALT
%token INT
%token IRET
%token CALL
%token RET
%token JMP
%token BEQ
%token BNE
%token BGT
%token PUSH
%token POP
%token XCHG 
%token ADD
%token SUB
%token MUL
%token DIV
%token NOT
%token AND
%token OR
%token XOR
%token SHL
%token SHR
%token LD
%token ST
%token CSRRD
%token CSRWR

%token SECTION
%token END
%token SKIP
%token WORD
%token GLOBAL
%token EXTERN
%token EOL

%type <init_list> list_symbol_or_literal 
%type <init_list> list_symbol

%%

// This is the actual grammar that bison will parse.
input:
    | line input
;

line:
      label instruction EOL 
    | label directive EOL 
    | instruction EOL
    | directive EOL
    | label EOL 
    | EOL
;

list_symbol_or_literal:
      SYMBOL { Operand node{typeid($1).name(), $1}; $$ = new vector<Operand>; $$->push_back(node); }
    | LITERAL { Operand node{typeid($1).name(), new int($1)}; $$ = new vector<Operand>; $$->push_back(node); } 
    | list_symbol_or_literal ',' SYMBOL { Operand node{typeid($3).name(), $3}; $$->push_back(node); }
    | list_symbol_or_literal ',' LITERAL { Operand node{typeid($3).name(), new int($3)}; $$->push_back(node); }
    ;

list_symbol:
      SYMBOL { Operand node{typeid($1).name(), $1}; $$ = new vector<Operand>; $$->push_back(node); }
    | list_symbol ',' SYMBOL { Operand node{typeid($3).name(), $3}; $$->push_back(node); }
    ;

label:
      SYMBOL ':' { if (Directives::defineLabel($1)) YYABORT; free($1); }

directive:
      SECTION SYMBOL { Directives::sectionDirective($2); }
    | SKIP LITERAL { Directives::skipDirective($2); }
    | WORD list_symbol_or_literal { Directives::wordDirective($2); delete $2; }
    | GLOBAL list_symbol { Directives::globalDirective($2); delete $2; }
    | EXTERN list_symbol { Directives::externDirective($2); delete $2; }
    | END { YYACCEPT; }

instruction:
      HALT { Instructions::halt(); }
    | INT  { Instructions::interrupt(); }
    | RET  { Instructions::pop(15); }
    | IRET { Instructions::iret(); }
    | CALL LITERAL { Instructions::call((uint32_t) $2); }
    | CALL SYMBOL  { Instructions::call(std::string($2)); free($2); }
    | JMP LITERAL { Instructions::jump(MOD_JMP::JMP, 0, 0, 0, (uint32_t) $2); }
    | JMP SYMBOL  { Instructions::jump(MOD_JMP::JMP, 0, 0, 0, std::string($2)); free($2); }
    | BEQ GPR ',' GPR ',' LITERAL { Instructions::jump(MOD_JMP::BEQ, 0, (uint8_t) $2, (uint8_t) $4, (uint32_t) $6); }
    | BEQ GPR ',' GPR ',' SYMBOL  { Instructions::jump(MOD_JMP::BEQ, 0, (uint8_t) $2, (uint8_t) $4, std::string($6)); free($6); }
    | BNE GPR ',' GPR ',' LITERAL { Instructions::jump(MOD_JMP::BNE, 0, (uint8_t) $2, (uint8_t) $4, (uint32_t) $6); }
    | BNE GPR ',' GPR ',' SYMBOL  { Instructions::jump(MOD_JMP::BNE, 0, (uint8_t) $2, (uint8_t) $4, std::string($6)); free($6); }
    | BGT GPR ',' GPR ',' LITERAL { Instructions::jump(MOD_JMP::BGT, 0, (uint8_t) $2, (uint8_t) $4, (uint32_t) $6); }
    | BGT GPR ',' GPR ',' SYMBOL  { Instructions::jump(MOD_JMP::BGT, 0, (uint8_t) $2, (uint8_t) $4, std::string($6)); free($6);}
    | PUSH GPR { Instructions::push((uint8_t) $2); }
    | POP GPR  { Instructions::pop((uint8_t) $2); }
    | XCHG GPR ',' GPR { Instructions::exchange((uint8_t) $2, (uint8_t) $4); }
    | ADD GPR ',' GPR { Instructions::arithmetic_logic_shift(OP_CODE::AR, MOD_ALU::ADD, (uint8_t) $2, (uint8_t) $4); }
    | SUB GPR ',' GPR { Instructions::arithmetic_logic_shift(OP_CODE::AR, MOD_ALU::SUB, (uint8_t) $2, (uint8_t) $4); }
    | MUL GPR ',' GPR { Instructions::arithmetic_logic_shift(OP_CODE::AR, MOD_ALU::MUL, (uint8_t) $2, (uint8_t) $4); }
    | DIV GPR ',' GPR { Instructions::arithmetic_logic_shift(OP_CODE::AR, MOD_ALU::DIV, (uint8_t) $2, (uint8_t) $4); }
    | NOT GPR         { Instructions::arithmetic_logic_shift(OP_CODE::LOG, MOD_ALU::NOT, (uint8_t) $2, (uint8_t) $2); }
    | AND GPR ',' GPR { Instructions::arithmetic_logic_shift(OP_CODE::LOG, MOD_ALU::AND, (uint8_t) $2, (uint8_t) $4); }
    | OR GPR ',' GPR  { Instructions::arithmetic_logic_shift(OP_CODE::LOG, MOD_ALU::OR, (uint8_t) $2, (uint8_t) $4); }
    | XOR GPR ',' GPR { Instructions::arithmetic_logic_shift(OP_CODE::LOG, MOD_ALU::XOR, (uint8_t) $2, (uint8_t) $4); }
    | SHL GPR ',' GPR { Instructions::arithmetic_logic_shift(OP_CODE::SHF, MOD_ALU::SHL, (uint8_t) $2, (uint8_t) $4); }
    | SHR GPR ',' GPR { Instructions::arithmetic_logic_shift(OP_CODE::SHF, MOD_ALU::SHR, (uint8_t) $2, (uint8_t) $4); }
    | LD '$' LITERAL ',' GPR             { Instructions::load(ADDR::IMMEDIATE, (uint8_t) $5, 0, (uint32_t) $3); }
    | LD '$' SYMBOL ',' GPR              { Instructions::load(ADDR::IMMEDIATE, (uint8_t) $5, 0, std::string($3)); free($3); }
    | LD LITERAL ',' GPR                 { Instructions::load(ADDR::MEM_DIR, (uint8_t) $4, 0, (uint32_t) $2); }
    | LD SYMBOL ',' GPR                  { Instructions::load(ADDR::MEM_DIR, (uint8_t) $4, 0, std::string($2)); free($2); }
    | LD GPR ',' GPR                     { Instructions::load(ADDR::REG_DIR, (uint8_t) $4, (uint8_t) $2, 0); }
    | LD '[' GPR ']' ',' GPR             { Instructions::load(ADDR::REG_IND, (uint8_t) $6, (uint8_t) $3, 0); }
    | LD '[' GPR '+' LITERAL ']' ',' GPR { Instructions::load(ADDR::REG_IND_OFF, (uint8_t) $8, (uint8_t) $3, (uint32_t) $5); }
    | ST GPR ',' '$' LITERAL             { Instructions::store(ADDR::IMMEDIATE, 0, 0, (uint8_t) $2, (uint32_t) $5); }
    | ST GPR ',' '$' SYMBOL              { Instructions::store(ADDR::IMMEDIATE, 0, 0, (uint8_t) $2, std::string($5)); free($5); }
    | ST GPR ',' LITERAL                 { Instructions::store(ADDR::MEM_DIR, 0, 0, (uint8_t) $2, (uint32_t) $4); }
    | ST GPR ',' SYMBOL                  { Instructions::store(ADDR::MEM_DIR, 0, 0, (uint8_t) $2, std::string($4)); free($4); }
    | ST GPR ',' GPR                     { Instructions::store(ADDR::REG_DIR, (uint8_t) $2, (uint8_t) $4, 0, 0); }
    | ST GPR ',' '[' GPR ']'             { Instructions::store(ADDR::REG_IND, (uint8_t) $5, 0, (uint8_t) $2, 0); }
    | ST GPR ',' '[' GPR '+' LITERAL ']' { Instructions::store(ADDR::REG_IND_OFF, (uint8_t) $5, 0, (uint8_t) $2, (uint32_t) $7); }
    | CSRRD CSR ',' GPR { Instructions::csr_read((uint8_t) $2, (uint8_t) $4); }
    | CSRWR GPR ',' CSR { Instructions::csr_write((uint8_t) $2, (uint8_t) $4); }
;

%%

void yyerror(const char *s) {
    cout << std::dec << "Error at line " << line << ": " << s << endl;
    exit(-1);
}
