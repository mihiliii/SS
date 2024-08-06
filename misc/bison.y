
%{
    #include <cstdio>
    #include <iostream>
    #include "../inc/Assembler.hpp"
    #include "../inc/Instructions.hpp"
    #include "../inc/Directives.hpp"

    using namespace std;

    // Declare stuff from Flex that Bison needs to know about:
    extern int yylex();
    extern int yyparse();
    extern FILE *yyin;
 
    void yyerror(const char *s);
%}

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

%token COMMA

%%

// This is the actual grammar that bison will parse.
input:
    | line input
;

line:
    instruction | directive
;

directive:
      SECTION STRING { cout << "SECTION " << $2 << endl; Directives::dSection($2);  }
    | END { cout << "END " << endl; Directives::dEnd(); }
    | SKIP NUMBER { cout << "SKIP " << $2 << endl; Directives::dSkip($2); }

instruction:
      HALT { Instructions::iHALT(); cout << $1 << endl; free($1); }
    | INT  { cout << "INT " << endl; free($1); }
    | IRET { cout << "IRET " << endl; free($1); }
    | CALL REGISTER { cout << "CALL " << $2 << endl; free($1); }
    | CALL NUMBER { cout << "CALL " << $2 << endl; }
    | RET { cout << "RET " << endl; free($1); }
    | JMP REGISTER { cout << "JMP " << $2 << endl; }
    | JMP NUMBER { cout << "JMP " << $2 << endl; }
    // missing jump instructions...
    | PUSH REGISTER { cout << "PUSH " << $2 << endl; }
    | POP REGISTER { cout << "POP " << $2 << endl; }
    | XCHG REGISTER COMMA REGISTER { cout << "XCHG " << $2 << ", " << $4 << endl; }
    | ADD REGISTER COMMA REGISTER { cout << "ADD " << $2 << ", " << $4 << endl; Instructions::arithmetic($1, (uint8_t) $2, (uint8_t) $4); free($1);}
    | SUB REGISTER COMMA REGISTER { cout << "SUB " << $2 << ", " << $4 << endl; }
    | MUL REGISTER COMMA REGISTER { cout << "MUL " << $2 << ", " << $4 << endl; }
    | DIV REGISTER COMMA REGISTER { cout << "DIV " << $2 << ", " << $4 << endl; }
    | NOT REGISTER { cout << "NOT " << $2 << endl; }
    | AND REGISTER COMMA REGISTER { cout << "AND " << $2 << ", " << $4 << endl; }
    | OR REGISTER COMMA REGISTER { cout << "OR " << $2 << ", " << $4 << endl; }
    | XOR REGISTER COMMA REGISTER { cout << "XOR " << $2 << ", " << $4 << endl; }
    | SHL REGISTER COMMA REGISTER { cout << "SHL " << $2 << ", " << $4 << endl; }
    | SHR REGISTER COMMA REGISTER { cout << "SHR " << $2 << ", " << $4 << endl; }
    | LD REGISTER COMMA REGISTER { cout << "LD " << $2 << ", " << $4 << endl; }
    | LD NUMBER COMMA REGISTER { cout << "LD " << $2 << ", " << $4 << endl; }
    | ST REGISTER COMMA REGISTER { cout << "ST " << $2 << ", " << $4 << endl; }
    | ST REGISTER COMMA NUMBER { cout << "ST " << $2 << ", " << $4 << endl; }
;

%%

void yyerror(const char *s) {
    cout << "Parse error! Message: " << s << endl;
    // might as well halt now:
    exit(-1);
}
