%{
    #include <cstdio>
    #include <iostream>
    #include <cmath>
    #include "../inc/Assembler/Assembler.hpp"

    extern int yylex();
    extern int yyparse();
    extern FILE *yyin;
 
    void yyerror(const char *s);

    extern Assembler* assembler;
    extern uint32_t line;
%}

%code requires {
    #include <vector>
    #include <cstdint>
    struct Operand;
    enum struct REG;
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
    REG reg_val;
    std::vector<Operand>* init_list;
}

// Define the "terminal symbol" token types I'm going to use (in CAPS
// by convention), and associate each with a field of the %union:
%token <uint_val> LITERAL
%token <s_val> SYMBOL
%token <reg_val> GPR
%token <reg_val> CSR

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
    SYMBOL
    {
        Operand node{Symbol($1)};
        $$ = new std::vector<Operand>();
        $$->push_back(node);
        free($1);
    }
  | LITERAL
    {
        Operand node{Literal($1)};
        $$ = new std::vector<Operand>();
        $$->push_back(node);
    }
  | list_symbol_or_literal ',' SYMBOL
    {
        Operand node{Symbol($3)};
        $$->push_back(node);
        free($3);
    }
  | list_symbol_or_literal ',' LITERAL
    {
        Operand node{Literal($3)};
        $$->push_back(node);
    }
;

list_symbol:
    SYMBOL
    {
        Operand node{Symbol($1)};
        $$ = new std::vector<Operand>();
        $$->push_back(node);
        free($1);
    }
  | list_symbol ',' SYMBOL
    {
        Operand node{Symbol($3)};
        $$->push_back(node);
        free($3);
    }
;

label:
    SYMBOL ':' {
        assembler->define_label($1);
        free($1);
    }
;

directive:
    SECTION SYMBOL
    {
        assembler->section_dir($2);
    }
  | SKIP LITERAL
    {
        assembler->skip_dir($2);
    }
  | WORD list_symbol_or_literal
    {
        assembler->word_dir(*$2);
        delete $2;
    }
  | GLOBAL list_symbol
    {
        assembler->global_dir(*$2);
        delete $2;
    }
  | EXTERN list_symbol
    {
        assembler->extern_dir(*$2);
        delete $2;
    }
  | END
    {
        YYACCEPT;
    }
;

instruction:
    HALT                                { assembler->halt(); }
  | INT                                 { assembler->interrupt(); }
  | RET                                 { assembler->pop(REG::PC); }
  | IRET                                { assembler->iret(); }
  | CALL LITERAL                        { assembler->call($2); }
  | CALL SYMBOL                         { assembler->call($2); free($2); }
  | JMP LITERAL                         { assembler->jump(MOD::JMP, REG::R0, REG::R0, REG::R0, $2); }
  | JMP SYMBOL                          { assembler->jump(MOD::JMP, REG::R0, REG::R0, REG::R0, $2); free($2); }
  | BEQ GPR ',' GPR ',' LITERAL         { assembler->jump(MOD::BEQ, REG::R0, $2, $4, $6); }
  | BEQ GPR ',' GPR ',' SYMBOL          { assembler->jump(MOD::BEQ, REG::R0, $2, $4, $6); free($6); }
  | BNE GPR ',' GPR ',' LITERAL         { assembler->jump(MOD::BNE, REG::R0, $2, $4, $6); }
  | BNE GPR ',' GPR ',' SYMBOL          { assembler->jump(MOD::BNE, REG::R0, $2, $4, $6); free($6); }
  | BGT GPR ',' GPR ',' LITERAL         { assembler->jump(MOD::BGT, REG::R0, $2, $4, $6); }
  | BGT GPR ',' GPR ',' SYMBOL          { assembler->jump(MOD::BGT, REG::R0, $2, $4, $6); free($6);}
  | PUSH GPR                            { assembler->push($2); }
  | POP GPR                             { assembler->pop($2); }
  | XCHG GPR ',' GPR                    { assembler->exchange($2, $4); }
  | ADD GPR ',' GPR                     { assembler->arithmetic_logic_shift(OC::AR, MOD::AR_ADD, $2, $4); }
  | SUB GPR ',' GPR                     { assembler->arithmetic_logic_shift(OC::AR, MOD::AR_SUB, $2, $4); }
  | MUL GPR ',' GPR                     { assembler->arithmetic_logic_shift(OC::AR, MOD::AR_MUL, $2, $4); }
  | DIV GPR ',' GPR                     { assembler->arithmetic_logic_shift(OC::AR, MOD::AR_DIV, $2, $4); }
  | NOT GPR                             { assembler->arithmetic_logic_shift(OC::LOG, MOD::LOG_NOT, $2, $2); }
  | AND GPR ',' GPR                     { assembler->arithmetic_logic_shift(OC::LOG, MOD::LOG_AND, $2, $4); }
  | OR GPR ',' GPR                      { assembler->arithmetic_logic_shift(OC::LOG, MOD::LOG_OR, $2, $4); }
  | XOR GPR ',' GPR                     { assembler->arithmetic_logic_shift(OC::LOG, MOD::LOG_XOR, $2, $4); }
  | SHL GPR ',' GPR                     { assembler->arithmetic_logic_shift(OC::SHF, MOD::SHF_SHL, $2, $4); }
  | SHR GPR ',' GPR                     { assembler->arithmetic_logic_shift(OC::SHF, MOD::SHF_SHR, $2, $4); }
  | LD '$' LITERAL ',' GPR              { assembler->load(IF_ADDR::IMMEDIATE, $5, REG::R0, $3); }
  | LD '$' SYMBOL ',' GPR               { assembler->load(IF_ADDR::IMMEDIATE, $5, REG::R0, $3); free($3); }
  | LD LITERAL ',' GPR                  { assembler->load(IF_ADDR::MEM_DIR, $4, REG::R0, $2); }
  | LD SYMBOL ',' GPR                   { assembler->load(IF_ADDR::MEM_DIR, $4, REG::R0, $2); free($2); }
  | LD GPR ',' GPR                      { assembler->load(IF_ADDR::REG_DIR, $4, $2, 0); }
  | LD '[' GPR ']' ',' GPR              { assembler->load(IF_ADDR::REG_IND, $6, $3, 0); }
  | LD '[' GPR '+' LITERAL ']' ',' GPR  { assembler->load(IF_ADDR::REG_IND_OFF, $8, $3, $5); }
  | ST GPR ',' '$' LITERAL              { assembler->store(IF_ADDR::IMMEDIATE, REG::R0, REG::R0, $2, $5); }
  | ST GPR ',' '$' SYMBOL               { assembler->store(IF_ADDR::IMMEDIATE, REG::R0, REG::R0, $2, $5); free($5); }
  | ST GPR ',' LITERAL                  { assembler->store(IF_ADDR::MEM_DIR, REG::R0, REG::R0, $2, $4); }
  | ST GPR ',' SYMBOL                   { assembler->store(IF_ADDR::MEM_DIR, REG::R0, REG::R0, $2, $4); free($4); }
  | ST GPR ',' GPR                      { assembler->store(IF_ADDR::REG_DIR, $2, $4, REG::R0, 0); }
  | ST GPR ',' '[' GPR ']'              { assembler->store(IF_ADDR::REG_IND, $5, REG::R0, $2, 0); }
  | ST GPR ',' '[' GPR '+' LITERAL ']'  { assembler->store(IF_ADDR::REG_IND_OFF, $5, REG::R0, $2, $7); }
  | CSRRD CSR ',' GPR                   { assembler->csr_read($2, $4); }
  | CSRWR GPR ',' CSR                   { assembler->csr_write($2, $4); }
;

%%

void yyerror(const char *s) {
    std::cout << std::dec << "Error at line " << line << ": " << s << std::endl;
    exit(-1);
}
