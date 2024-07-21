
%{
    #include <cstdio>
    #include <iostream>
    #include "../inc/Assembler.hpp"
    #include "../inc/Instructions.hpp"

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

%token <sval> REGISTER
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

%token COMMA

%%

// This is the actual grammar that bison will parse.
input:
    | line input
;

line:
    instruction
;

instruction:
      HALT { Instructions::iHALT(); cout << $1 << endl; free($1); }
    | INT  { cout << "INT " << endl; free($1); }
    | IRET { cout << "IRET " << endl; free($1); }
    | CALL REGISTER { cout << "CALL " << $2 << endl; free($1); }
    | CALL NUMBER { cout << "CALL " << $2 << endl; }
    | RET { cout << "RET " << endl; free($1); }
    | JMP REGISTER { cout << "JMP " << $2 << endl; free($2); }
    | JMP NUMBER { cout << "JMP " << $2 << endl; }
    // missing jump instructions...
    | PUSH REGISTER { cout << "PUSH " << $2 << endl; free($2); }
    | POP REGISTER { cout << "POP " << $2 << endl; free($2); }
    | XCHG REGISTER COMMA REGISTER { cout << "XCHG " << $2 << ", " << $4 << endl; free($2); free($4); }
    | ADD REGISTER COMMA REGISTER { cout << "ADD " << $2 << ", " << $4 << endl; free($2); free($4); }
    | SUB REGISTER COMMA REGISTER { cout << "SUB " << $2 << ", " << $4 << endl; free($2); free($4); }
    | MUL REGISTER COMMA REGISTER { cout << "MUL " << $2 << ", " << $4 << endl; free($2); free($4); }
    | DIV REGISTER COMMA REGISTER { cout << "DIV " << $2 << ", " << $4 << endl; free($2); free($4); }
    | NOT REGISTER { cout << "NOT " << $2 << endl; free($2); }
    | AND REGISTER COMMA REGISTER { cout << "AND " << $2 << ", " << $4 << endl; free($2); free($4); }
    | OR REGISTER COMMA REGISTER { cout << "OR " << $2 << ", " << $4 << endl; free($2); free($4); }
    | XOR REGISTER COMMA REGISTER { cout << "XOR " << $2 << ", " << $4 << endl; free($2); free($4); }
    | SHL REGISTER COMMA REGISTER { cout << "SHL " << $2 << ", " << $4 << endl; free($2); free($4); }
    | SHR REGISTER COMMA REGISTER { cout << "SHR " << $2 << ", " << $4 << endl; free($2); free($4); }
    | LD REGISTER COMMA REGISTER { cout << "LD " << $2 << ", " << $4 << endl; free($2); free($4); }
    | LD NUMBER COMMA REGISTER { cout << "LD " << $2 << ", " << $4 << endl; free($4); }
    | ST REGISTER COMMA REGISTER { cout << "ST " << $2 << ", " << $4 << endl; free($2); free($4); }
    | ST REGISTER COMMA NUMBER { cout << "ST " << $2 << ", " << $4 << endl; free($2); }
;

%%

void yyerror(const char *s) {
    cout << "Parse error! Message: " << s << endl;
    // might as well halt now:
    exit(-1);
}
