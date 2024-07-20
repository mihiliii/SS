%{  
    #include "bison.tab.h"
    #include "../inc/Assembler.hpp"
    
    extern int yyerror(const char* s);
%}
%option noyywrap

/* directives: */
GLOBAL ".global"
EXTERN ".extern"
SECTION ".section"
WORD ".word"
SKIP ".skip"
END ".end"

/* instructions: */
HALT "halt"
INT "int"
IRET "iret"
CALL "call"
RET "ret"
JMP "jmp"
BEQ "beq"
BNE "bne"
BGT "bgt"
PUSH "push"
POP "pop"
ADD "add"
SUB "sub"
MUL "mul"
DIV "div"
NOT "not"
AND "and"
OR "or"
XOR "xor"
SHL "shl"
SHR "shr"
LD "ld"
ST "st"
CSSRD "cssrd"
CSRWR "csrwr"

%%

"#"[^\n]*\n                 { printf("%s\n", yytext); };
","                         { return COMMA; }
{HALT}                      { yylval.sval = strdup(yytext); return HALT; }
{INT}                       { yylval.sval = strdup(yytext); return INT; 
{JMP}
{ADD}|{SUB}|{MUL}|{DIV}     { yylval.sval = strdup(yytext); return OPCODE2; }
%r[0-9]+                    { yylval.sval = strdup(yytext); return REGISTER; }
0[xX][0-9a-fA-F]+|[0-9]+    { yylval.ival = (int) strtol(yytext, NULL, 0); return NUMBER; }
[a-zA-Z0-9]+                { yylval.sval = strdup(yytext); return STRING; }
\n                          { Assembler::increaseLocationCounter(); }
[ \t]+                      ;
.                           { printf("Unknown character: %s\n", yytext); }

%%