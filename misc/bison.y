
%{
    #include "../inc/Directives.hpp"
    #include <cstdio>
    #include <iostream>
    #include <cmath>
    #include "../inc/Assembler.hpp"
    #include "../inc/Instructions.hpp"
    #include "../inc/SymbolTable.hpp"

    using namespace std;

    // Declare stuff from Flex that Bison needs to know about:
    extern int yylex();
    extern int yyparse();
    extern FILE *yyin;
 
    void yyerror(const char *s);

%}

%code requires {
    #include <vector>
    #include <cstdint>
    struct operand;
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
    std::vector<operand> *init_list;
}

// Define the "terminal symbol" token types I'm going to use (in CAPS
// by convention), and associate each with a field of the %union:
%token <uint_val> LITERAL
%token <s_val> STRING 

%token <uint_val> REGISTER
%token <s_val> HALT
%token <s_val> INT
%token <s_val> IRET
%token <s_val> CALL
%token <s_val> RET
%token <s_val> JMP
%token <s_val> BEQ
%token <s_val> BNE
%token <s_val> BGT
%token <s_val> PUSH
%token <s_val> POP
%token <s_val> XCHG 
%token <s_val> ADD
%token <s_val> SUB
%token <s_val> MUL
%token <s_val> DIV
%token <s_val> NOT
%token <s_val> AND
%token <s_val> OR
%token <s_val> XOR
%token <s_val> SHL
%token <s_val> SHR
%token <s_val> LD
%token <s_val> ST
%token <s_val> CSSRD
%token <s_val> CSRWR

%token <s_val> SECTION
%token <s_val> END
%token <s_val> SKIP
%token <s_val> WORD
%token <s_val> GLOBAL
%token <s_val> EXTERN

%type <init_list> list_symbol_or_literal 
%type <init_list> list_symbol

%%

// This is the actual grammar that bison will parse.
input:
    | line input
;

line:
    instruction | directive | label
;

list_symbol_or_literal:
      STRING { operand node{typeid($1).name(), $1}; $$ = new vector<operand>; $$->push_back(node); }
    | LITERAL { operand node{typeid($1).name(), new int($1)}; $$ = new vector<operand>; $$->push_back(node); } 
    | list_symbol_or_literal ',' STRING { operand node{typeid($3).name(), $3}; $$->push_back(node); }
    | list_symbol_or_literal ',' LITERAL { operand node{typeid($3).name(), new int($3)}; $$->push_back(node); }
    ;

list_symbol:
      STRING { operand node{typeid($1).name(), $1}; $$ = new vector<operand>; $$->push_back(node); }
    | list_symbol ',' STRING { operand node{typeid($3).name(), $3}; $$->push_back(node); }
    ;

label:
      STRING ':' { cout << "LABEL " << $1 << endl; if (Directives::defineLabel($1)) YYABORT; free($1); }

directive:
      SECTION STRING { cout << "SECTION " << $2 << endl; Directives::sectionDirective($2); }
    | SKIP LITERAL { cout << "SKIP " << $2 << endl; Directives::skipDirective($2); }
    | WORD list_symbol_or_literal { cout << "WORD "; for (auto value: *($2)) std::cout << std::hex << "0x" << *(uint32_t*) value.value << " "; std::cout << std::endl; Directives::wordDirective($2); delete $2; }
    | GLOBAL list_symbol { cout << "GLOBAL "; for (auto symbol: *($2)) std::cout << symbol.value << " "; std::cout << std::endl; Directives::globalDirective($2); delete $2; }
    | EXTERN list_symbol { cout << "EXTERN "; for (auto symbol: *($2)) std::cout << symbol.value << " "; std::cout << std::endl; Directives::externDirective($2); delete $2; }
    | END { cout << "END " << endl; YYACCEPT; }

instruction:
      HALT { Instructions::halt(); cout << $1 << endl; free($1); }
    | INT  { cout << "INT " << endl; free($1); }
    | IRET { cout << "IRET " << endl; free($1); }
    | CALL REGISTER { cout << "CALL " << $2 << endl; free($1); }
    | CALL LITERAL { cout << "CALL " << $2 << endl; }
    | RET { cout << "RET " << endl; free($1); }
    | JMP STRING { cout << "JMP " << $2 << endl; Instructions::jump(MOD_JMP::JMP, 15, 0, 0, std::string($2)); }
    | JMP LITERAL { cout << "JMP " << $2 << endl; Instructions::jump(MOD_JMP::JMP, 15, 0, 0, (uint32_t) $2); }
    | BEQ REGISTER ',' REGISTER ',' STRING { cout << "BEQ " << $2 << ", " << $4 << ", " << $6 << endl; Instructions::jump(MOD_JMP::BEQ, 15, (uint8_t) $2, (uint8_t) $4, std::string($6)); }
    | BEQ REGISTER ',' REGISTER ',' LITERAL { cout << "BEQ " << $2 << ", " << $4 << ", " << $6 << endl; Instructions::jump(MOD_JMP::BEQ, 15, (uint8_t) $2, (uint8_t) $4, (uint32_t) $6); }
    | BNE REGISTER ',' REGISTER ',' STRING { cout << "BNE " << $2 << ", " << $4 << ", " << $6 << endl; Instructions::jump(MOD_JMP::BNE, 15, (uint8_t) $2, (uint8_t) $4, std::string($6)); }
    | BNE REGISTER ',' REGISTER ',' LITERAL { cout << "BNE " << $2 << ", " << $4 << ", " << $6 << endl; Instructions::jump(MOD_JMP::BNE, 15, (uint8_t) $2, (uint8_t) $4, (uint32_t) $6); }
    | BGT REGISTER ',' REGISTER ',' STRING { cout << "BGT " << $2 << ", " << $4 << ", " << $6 << endl; Instructions::jump(MOD_JMP::BGT, 15, (uint8_t) $2, (uint8_t) $4, std::string($6)); }
    | BGT REGISTER ',' REGISTER ',' LITERAL { cout << "BGT " << $2 << ", " << $4 << ", " << $6 << endl; Instructions::jump(MOD_JMP::BGT, 15, (uint8_t) $2, (uint8_t) $4, (uint32_t) $6); }
    | PUSH REGISTER { cout << "PUSH " << $2 << endl; }
    | POP REGISTER { cout << "POP " << $2 << endl; }
    | XCHG REGISTER ',' REGISTER { cout << "XCHG " << $2 << ", " << $4 << endl; }
    | ADD REGISTER ',' REGISTER { cout << "ADD " << $2 << ", " << $4 << endl; Instructions::arithmetic_logic_shift(OP_CODE::AR, MOD_ALU::ADD, (uint8_t) $2, (uint8_t) $4); free($1); }
    | SUB REGISTER ',' REGISTER { cout << "SUB " << $2 << ", " << $4 << endl; Instructions::arithmetic_logic_shift(OP_CODE::AR, MOD_ALU::SUB, (uint8_t) $2, (uint8_t) $4); free($1); }
    | MUL REGISTER ',' REGISTER { cout << "MUL " << $2 << ", " << $4 << endl; Instructions::arithmetic_logic_shift(OP_CODE::AR, MOD_ALU::MUL, (uint8_t) $2, (uint8_t) $4); free($1); }
    | DIV REGISTER ',' REGISTER { cout << "DIV " << $2 << ", " << $4 << endl; Instructions::arithmetic_logic_shift(OP_CODE::AR, MOD_ALU::DIV, (uint8_t) $2, (uint8_t) $4); free($1); }
    | NOT REGISTER { cout << "NOT " << $2 << endl; Instructions::arithmetic_logic_shift(OP_CODE::LOG, MOD_ALU::NOT, (uint8_t) $2, (uint8_t) $2); free($1); }
    | AND REGISTER ',' REGISTER { cout << "AND " << $2 << ", " << $4 << endl; Instructions::arithmetic_logic_shift(OP_CODE::LOG, MOD_ALU::AND, (uint8_t) $2, (uint8_t) $4); free($1); }
    | OR REGISTER ',' REGISTER { cout << "OR " << $2 << ", " << $4 << endl; Instructions::arithmetic_logic_shift(OP_CODE::LOG, MOD_ALU::OR, (uint8_t) $2, (uint8_t) $4); free($1); }
    | XOR REGISTER ',' REGISTER { cout << "XOR " << $2 << ", " << $4 << endl; Instructions::arithmetic_logic_shift(OP_CODE::LOG, MOD_ALU::XOR, (uint8_t) $2, (uint8_t) $4); free($1); }
    | SHL REGISTER ',' REGISTER { cout << "SHL " << $2 << ", " << $4 << endl; Instructions::arithmetic_logic_shift(OP_CODE::SHF, MOD_ALU::SHL, (uint8_t) $2, (uint8_t) $4); free($1); }
    | SHR REGISTER ',' REGISTER { cout << "SHR " << $2 << ", " << $4 << endl; Instructions::arithmetic_logic_shift(OP_CODE::SHF, MOD_ALU::SHR, (uint8_t) $2, (uint8_t) $4); free($1); }
    | LD '$' LITERAL ',' REGISTER { cout << "LD " << $3 << ", " << $5 << endl; Instructions::load(LD_ADDR::LITERAL_GPR, (uint8_t) $5, 0, 0, (uint32_t) $3); }
    | LD '$' STRING ',' REGISTER { cout << "LD " << $3 << ", " << $5 << endl; }
    | ST REGISTER ',' REGISTER { cout << "ST " << $2 << ", " << $4 << endl; }
    | ST REGISTER ',' LITERAL { cout << "ST " << $2 << ", " << $4 << endl; }
;

%%

void yyerror(const char *s) {
    cout << "Parse error! Message: " << s << endl;
    // might as well halt now:
    exit(-1);
}
