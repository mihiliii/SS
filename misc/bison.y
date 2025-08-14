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
    line input
;

line:
    label instruction EOL
    label directive EOL
    instruction EOL
    directive EOL
    label EOL
    EOL
;

list_symbol_or_literal:
    SYMBOL
    {
        Operand node{OPERAND_TYPE::SYMBOL, new std::string($1)};
        $$ = new vector<Operand>;
        $$->push_back(node);
        free($1);
    }
    LITERAL
    {
        Operand node{OPERAND_TYPE::LITERAL, new int($1)};
        $$ = new vector<Operand>;
        $$->push_back(node);
    }
    list_symbol_or_literal ',' SYMBOL
    {
        Operand node{OPERAND_TYPE::SYMBOL, new std::string($3)};
        $$->push_back(node);
        free($3);
    }
    list_symbol_or_literal ',' LITERAL
    {
        Operand node{OPERAND_TYPE::LITERAL, new int($3)};
        $$->push_back(node);
    }
;

list_symbol:
    SYMBOL
    {
        Operand node{OPERAND_TYPE::SYMBOL, new std::string($1)};
        $$ = new vector<Operand>;
        $$->push_back(node);
        free($1);
    }
    list_symbol ',' SYMBOL {
        Operand node{OPERAND_TYPE::SYMBOL, new std::string($3)};
        $$->push_back(node);
        free($1);
    }
;

label:
    SYMBOL ':' {
        assembler->define_label($1);
        free($1);
    }
;

// FIX: need to delete all newly allocated memory in the directives

directive:
    SECTION SYMBOL
    {
        assembler->sectionDirective($2);
    }
    SKIP LITERAL
    {
        assembler->skipDirective($2);
    }
    WORD list_symbol_or_literal
    {
        assembler->wordDirective($2);
        for (auto it : *$2) {
            delete it->value;
        }
        delete $2;
    }
    GLOBAL list_symbol
    {
        assembler->globalDirective($2);
        for (auto it : *$2) {
            delete it->value;
        }
        delete $2;
    }
    EXTERN list_symbol
    {
        assembler->externDirective($2);
        for (auto it : *$2) {
            delete it.value;
        }
        delete $2;
    }
    END
    {
        YYACCEPT;
    }
;

// TODO: remove casts

instruction:
    HALT                                { assembler->halt(); }
    INT                                 { assembler->interrupt(); }
    RET                                 { assembler->pop(REG::PC); }
    IRET                                { assembler->iret(); }
    CALL LITERAL                        { assembler->call((uint32_t)$2); }
    CALL SYMBOL                         { assembler->call(std::string($2)); free($2); }
    JMP LITERAL                         { assembler->jump(MOD::JMP, REG::R0, REG::R0, REG::R0, (uint32_t) $2); }
    JMP SYMBOL                          { assembler->jump(MOD::JMP, REG::R0, REG::R0, REG::R0, std::string($2)); free($2); }
    BEQ GPR ',' GPR ',' LITERAL         { assembler->jump(MOD::BEQ, REG::R0, (uint8_t) $2, (uint8_t) $4, (uint32_t) $6); }
    BEQ GPR ',' GPR ',' SYMBOL          { assembler->jump(MOD::BEQ, REG::R0, (uint8_t) $2, (uint8_t) $4, std::string($6)); free($6); }
    BNE GPR ',' GPR ',' LITERAL         { assembler->jump(MOD::BNE, REG::R0, (uint8_t) $2, (uint8_t) $4, (uint32_t) $6); }
    BNE GPR ',' GPR ',' SYMBOL          { assembler->jump(MOD::BNE, REG::R0, (uint8_t) $2, (uint8_t) $4, std::string($6)); free($6); }
    BGT GPR ',' GPR ',' LITERAL         { assembler->jump(MOD::BGT, REG::R0, (uint8_t) $2, (uint8_t) $4, (uint32_t) $6); }
    BGT GPR ',' GPR ',' SYMBOL          { assembler->jump(MOD::BGT, REG::R0, (uint8_t) $2, (uint8_t) $4, std::string($6)); free($6);}
    PUSH GPR                            { assembler->push((uint8_t) $2); }
    POP GPR                             { assembler->pop((uint8_t) $2); }
    XCHG GPR ',' GPR                    { assembler->exchange((uint8_t) $2, (uint8_t) $4); }
    ADD GPR ',' GPR                     { assembler->arithmetic_logic_shift(OC::AR, MOD::ADD, (uint8_t) $2, (uint8_t) $4); }
    SUB GPR ',' GPR                     { assembler->arithmetic_logic_shift(OC::AR, MOD::SUB, (uint8_t) $2, (uint8_t) $4); }
    MUL GPR ',' GPR                     { assembler->arithmetic_logic_shift(OC::AR, MOD::MUL, (uint8_t) $2, (uint8_t) $4); }
    DIV GPR ',' GPR                     { assembler->arithmetic_logic_shift(OC::AR, MOD::DIV, (uint8_t) $2, (uint8_t) $4); }
    NOT GPR                             { assembler->arithmetic_logic_shift(OC::LOG, MOD::NOT, (uint8_t) $2, (uint8_t) $2); }
    AND GPR ',' GPR                     { assembler->arithmetic_logic_shift(OC::LOG, MOD::AND, (uint8_t) $2, (uint8_t) $4); }
    OR GPR ',' GPR                      { assembler->arithmetic_logic_shift(OC::LOG, MOD::OR, (uint8_t) $2, (uint8_t) $4); }
    XOR GPR ',' GPR                     { assembler->arithmetic_logic_shift(OC::LOG, MOD::XOR, (uint8_t) $2, (uint8_t) $4); }
    SHL GPR ',' GPR                     { assembler->arithmetic_logic_shift(OC::SHF, MOD::SHL, (uint8_t) $2, (uint8_t) $4); }
    SHR GPR ',' GPR                     { assembler->arithmetic_logic_shift(OC::SHF, MOD::SHR, (uint8_t) $2, (uint8_t) $4); }
    LD '$' LITERAL ',' GPR              { assembler->load(ADDR::IMMEDIATE, (REG) $5, REG::R0, (uint32_t) $3); }
    LD '$' SYMBOL ',' GPR               { assembler->load(ADDR::IMMEDIATE, (REG) $5, REG::R0, std::string($3)); free($3); }
    LD LITERAL ',' GPR                  { assembler->load(ADDR::MEM_DIR, (REG) $4, REG::R0, (uint32_t) $2); }
    LD SYMBOL ',' GPR                   { assembler->load(ADDR::MEM_DIR, (REG) $4, REG::R0, std::string($2)); free($2); }
    LD GPR ',' GPR                      { assembler->load(ADDR::REG_DIR, (REG) $4, (REG) $2, 0); }
    LD '[' GPR ']' ',' GPR              { assembler->load(ADDR::REG_IND, (REG) $6, (REG) $3, 0); }
    LD '[' GPR '+' LITERAL ']' ',' GPR  { assembler->load(ADDR::REG_IND_OFF, (REG) $8, (REG) $3, (uint32_t) $5); }
    ST GPR ',' '$' LITERAL              { assembler->store(ADDR::IMMEDIATE, REG::R0, REG::R0, (REG) $2, (uint32_t) $5); }
    ST GPR ',' '$' SYMBOL               { assembler->store(ADDR::IMMEDIATE, REG::R0, REG::R0, (REG) $2, std::string($5)); free($5); }
    ST GPR ',' LITERAL                  { assembler->store(ADDR::MEM_DIR, REG::R0, REG::R0, (REG) $2, (uint32_t) $4); }
    ST GPR ',' SYMBOL                   { assembler->store(ADDR::MEM_DIR, REG::R0, REG::R0, (REG) $2, std::string($4)); free($4); }
    ST GPR ',' GPR                      { assembler->store(ADDR::REG_DIR, (REG) $2, (REG) $4, REG::R0, 0); }
    ST GPR ',' '[' GPR ']'              { assembler->store(ADDR::REG_IND, (REG) $5, REG::R0, (REG) $2, 0); }
    ST GPR ',' '[' GPR '+' LITERAL ']'  { assembler->store(ADDR::REG_IND_OFF, (REG) $5, REG::R0, (REG) $2, (uint32_t) $7); }
    CSRRD CSR ',' GPR                   { assembler->csr_read((REG) $2, (REG) $4); }
    CSRWR GPR ',' CSR                   { assembler->csr_write((REG) $2, (REG) $4); }
;

%%

void yyerror(const char *s) {
    cout << std::dec << "Error at line " << line << ": " << s << endl;
    exit(-1);
}
