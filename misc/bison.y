
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
    struct init_list_node;
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
    int ival;
    char* sval;
    std::vector<init_list_node> *init_list;
}

// Define the "terminal symbol" token types I'm going to use (in CAPS
// by convention), and associate each with a field of the %union:
%token <ival> NUMBER
%token <sval> STRING 

%token <ival> REGISTER
%token <sval> HALT
%token <sval> INT
%token <sval> IRET
%token <sval> CALL
%token <sval> RET
%token <sval> JMP
%token <sval> BEQ
%token <sval> BNE
%token <sval> BGT
%token <sval> PUSH
%token <sval> POP
%token <sval> XCHG 
%token <sval> ADD
%token <sval> SUB
%token <sval> MUL
%token <sval> DIV
%token <sval> NOT
%token <sval> AND
%token <sval> OR
%token <sval> XOR
%token <sval> SHL
%token <sval> SHR
%token <sval> LD
%token <sval> ST
%token <sval> CSSRD
%token <sval> CSRWR

%token <sval> SECTION
%token <sval> END
%token <sval> SKIP
%token <sval> WORD
%token <sval> GLOBAL
%token <sval> EXTERN

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
      STRING { init_list_node node{typeid($1).name(), $1}; $$ = new vector<init_list_node>; $$->push_back(node); }
    | NUMBER { init_list_node node{typeid($1).name(), new int($1)}; $$ = new vector<init_list_node>; $$->push_back(node); } 
    | list_symbol_or_literal ',' STRING { init_list_node node{typeid($3).name(), $3}; $$->push_back(node); }
    | list_symbol_or_literal ',' NUMBER { init_list_node node{typeid($3).name(), new int($3)}; $$->push_back(node); }
    ;

list_symbol:
      STRING { init_list_node node{typeid($1).name(), $1}; $$ = new vector<init_list_node>; $$->push_back(node); }
    | list_symbol ',' STRING { init_list_node node{typeid($3).name(), $3}; $$->push_back(node); }
    ;

label:
    STRING ':' { 
        cout << "LABEL " << $1 << endl;
        Elf32_Sym* symbol = Assembler::symbol_table->findSymbol(std::string($1));
        if (symbol != nullptr)
            if (symbol->st_defined == true) {
                std::cout << "Symbol " << std::string($1) << " already defined!" << std::endl;
                YYABORT;
            }
            else
                Assembler::symbol_table->defineSymbol(symbol, Assembler::current_section->getLocationCounter()); 
        else
            Assembler::symbol_table->addSymbol($1, Assembler::current_section->getLocationCounter(), true);
        free($1);
    }

directive:
      SECTION STRING { cout << "SECTION " << $2 << endl; Directives::dSection($2);  }
    | END { cout << "END " << endl; YYACCEPT; }
    | SKIP NUMBER { cout << "SKIP " << $2 << endl; Directives::dSkip($2); }
    | WORD list_symbol_or_literal { cout << "WORD "; for (auto value: *($2)) std::cout << std::hex << "0x" << *(uint32_t*) value.value << " "; std::cout << std::endl; Directives::dWord($2); delete $2; }
    | GLOBAL list_symbol { cout << "GLOBAL "; for (auto symbol: *($2)) std::cout << symbol.value << " "; std::cout << std::endl; Directives::dGlobal($2); delete $2; }
    | EXTERN list_symbol { cout << "EXTERN "; for (auto symbol: *($2)) std::cout << symbol.value << " "; std::cout << std::endl; Directives::dExtern($2); delete $2; }

instruction:
      HALT { Instructions::haltIns(); cout << $1 << endl; free($1); }
    | INT  { cout << "INT " << endl; free($1); }
    | IRET { cout << "IRET " << endl; free($1); }
    | CALL REGISTER { cout << "CALL " << $2 << endl; free($1); }
    | CALL NUMBER { cout << "CALL " << $2 << endl; }
    | RET { cout << "RET " << endl; free($1); }
    | JMP STRING { 
        cout << "JMP " << $2 << endl; 
        Instructions::jumpIns(OP_CODE::JMP, 15, 0, 0, std::string($2));
    }
    | JMP NUMBER { 
        cout << "JMP " << $2 << endl;
        Instructions::jumpIns(OP_CODE::JMP, 15, 0, 0, (uint32_t) $2);
    }
    // missing jump instructions...
    | PUSH REGISTER { cout << "PUSH " << $2 << endl; }
    | POP REGISTER { cout << "POP " << $2 << endl; }
    | XCHG REGISTER ',' REGISTER { cout << "XCHG " << $2 << ", " << $4 << endl; }
    | ADD REGISTER ',' REGISTER { cout << "ADD " << $2 << ", " << $4 << endl; Instructions::arithmeticIns(MOD_ALU::ADD, (uint8_t) $2, (uint8_t) $4); free($1); }
    | SUB REGISTER ',' REGISTER { cout << "SUB " << $2 << ", " << $4 << endl; Instructions::arithmeticIns(MOD_ALU::SUB, (uint8_t) $2, (uint8_t) $4); free($1); }
    | MUL REGISTER ',' REGISTER { cout << "MUL " << $2 << ", " << $4 << endl; Instructions::arithmeticIns(MOD_ALU::MUL, (uint8_t) $2, (uint8_t) $4); free($1); }
    | DIV REGISTER ',' REGISTER { cout << "DIV " << $2 << ", " << $4 << endl; Instructions::arithmeticIns(MOD_ALU::DIV, (uint8_t) $2, (uint8_t) $4); free($1); }
    | NOT REGISTER { cout << "NOT " << $2 << endl; Instructions::logicIns(MOD_LOG::NOT, (uint8_t) $2, (uint8_t) $2); free($1); }
    | AND REGISTER ',' REGISTER { cout << "AND " << $2 << ", " << $4 << endl; Instructions::logicIns(MOD_LOG::AND, (uint8_t) $2, (uint8_t) $4); free($1); }
    | OR REGISTER ',' REGISTER { cout << "OR " << $2 << ", " << $4 << endl; Instructions::logicIns(MOD_LOG::OR, (uint8_t) $2, (uint8_t) $4); free($1); }
    | XOR REGISTER ',' REGISTER { cout << "XOR " << $2 << ", " << $4 << endl; Instructions::logicIns(MOD_LOG::XOR, (uint8_t) $2, (uint8_t) $4); free($1); }
    | SHL REGISTER ',' REGISTER { cout << "SHL " << $2 << ", " << $4 << endl; Instructions::shiftIns(MOD_SHF::SHL, (uint8_t) $2, (uint8_t) $4); free($1); }
    | SHR REGISTER ',' REGISTER { cout << "SHR " << $2 << ", " << $4 << endl; Instructions::shiftIns(MOD_SHF::SHR, (uint8_t) $2, (uint8_t) $4); free($1); }
    | LD REGISTER ',' REGISTER { cout << "LD " << $2 << ", " << $4 << endl; }
    | LD NUMBER ',' REGISTER { cout << "LD " << $2 << ", " << $4 << endl; }
    | ST REGISTER ',' REGISTER { cout << "ST " << $2 << ", " << $4 << endl; }
    | ST REGISTER ',' NUMBER { cout << "ST " << $2 << ", " << $4 << endl; }
;

%%

void yyerror(const char *s) {
    cout << "Parse error! Message: " << s << endl;
    // might as well halt now:
    exit(-1);
}
