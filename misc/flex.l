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
XCHG "xchg"
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

"#"[^\n]*\n                 { printf("%s", yytext); };
","                         { return COMMA; }
{HALT}                      { yylval.sval = strdup(yytext); return HALT; }
{INT}                       { yylval.sval = strdup(yytext); return INT; }
{IRET}                      { yylval.sval = strdup(yytext); return IRET; }
{CALL}                      { yylval.sval = strdup(yytext); return CALL; }
{RET}                       { yylval.sval = strdup(yytext); return RET; }
{JMP}                       { yylval.sval = strdup(yytext); return JMP; }
{BEQ}                       { yylval.sval = strdup(yytext); return BEQ; }
{BNE}                       { yylval.sval = strdup(yytext); return BNE; }
{BGT}                       { yylval.sval = strdup(yytext); return BGT; }
{PUSH}                      { yylval.sval = strdup(yytext); return PUSH; }
{POP}                       { yylval.sval = strdup(yytext); return POP; }
{XCHG}                      { yylval.sval = strdup(yytext); return XCHG; }
{ADD}                       { yylval.sval = strdup(yytext); return ADD; }
{SUB}                       { yylval.sval = strdup(yytext); return SUB; }
{MUL}                       { yylval.sval = strdup(yytext); return MUL; }
{DIV}                       { yylval.sval = strdup(yytext); return DIV; }      
{NOT}                       { yylval.sval = strdup(yytext); return NOT; }
{AND}                       { yylval.sval = strdup(yytext); return AND; }
{OR}                        { yylval.sval = strdup(yytext); return OR; }
{XOR}                       { yylval.sval = strdup(yytext); return XOR; }
{SHL}                       { yylval.sval = strdup(yytext); return SHL; }
{SHR}                       { yylval.sval = strdup(yytext); return SHR; }
{LD}                        { yylval.sval = strdup(yytext); return LD; }
{ST}                        { yylval.sval = strdup(yytext); return ST; }
{CSSRD}                     { yylval.sval = strdup(yytext); return CSSRD; }
{CSRWR}                     { yylval.sval = strdup(yytext); return CSRWR; }

{SECTION}                   { yylval.sval = strdup(yytext); return SECTION; }
{SKIP}                      { yylval.sval = strdup(yytext); return SKIP; } 

%r[0-9]|%r1[0-5]            { yylval.ival = (int) strtol(yytext + 2, NULL, 10); return REGISTER; }
0[xX][0-9a-fA-F]+|[0-9]+    { yylval.ival = (int) strtol(yytext, NULL, 0); return NUMBER; }
[a-zA-Z0-9]+                { yylval.sval = strdup(yytext); return STRING; }
\n                          { Assembler::increaseLocationCounter(4); }
[ \t]+                      ;
.                           { printf("Unknown character: %s\n", yytext); }

%%