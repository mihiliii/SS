%{  
    #include "bison.tab.h"
    #include "../inc/Assembler.hpp"
    
    extern int yyerror(const char* s);
%}
%option noyywrap
%option nounput

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
","                         { return ','; }
":"                         { return ':'; }
{HALT}                      { yylval.s_val = strdup(yytext); return HALT; }
{INT}                       { yylval.s_val = strdup(yytext); return INT; }
{IRET}                      { yylval.s_val = strdup(yytext); return IRET; }
{CALL}                      { yylval.s_val = strdup(yytext); return CALL; }
{RET}                       { yylval.s_val = strdup(yytext); return RET; }
{JMP}                       { yylval.s_val = strdup(yytext); return JMP; }
{BEQ}                       { yylval.s_val = strdup(yytext); return BEQ; }
{BNE}                       { yylval.s_val = strdup(yytext); return BNE; }
{BGT}                       { yylval.s_val = strdup(yytext); return BGT; }
{PUSH}                      { yylval.s_val = strdup(yytext); return PUSH; }
{POP}                       { yylval.s_val = strdup(yytext); return POP; }
{XCHG}                      { yylval.s_val = strdup(yytext); return XCHG; }
{ADD}                       { yylval.s_val = strdup(yytext); return ADD; }
{SUB}                       { yylval.s_val = strdup(yytext); return SUB; }
{MUL}                       { yylval.s_val = strdup(yytext); return MUL; }
{DIV}                       { yylval.s_val = strdup(yytext); return DIV; }      
{NOT}                       { yylval.s_val = strdup(yytext); return NOT; }
{AND}                       { yylval.s_val = strdup(yytext); return AND; }
{OR}                        { yylval.s_val = strdup(yytext); return OR; }
{XOR}                       { yylval.s_val = strdup(yytext); return XOR; }
{SHL}                       { yylval.s_val = strdup(yytext); return SHL; }
{SHR}                       { yylval.s_val = strdup(yytext); return SHR; }
{LD}                        { yylval.s_val = strdup(yytext); return LD; }
{ST}                        { yylval.s_val = strdup(yytext); return ST; }
{CSSRD}                     { yylval.s_val = strdup(yytext); return CSSRD; }
{CSRWR}                     { yylval.s_val = strdup(yytext); return CSRWR; }

{SECTION}                   { yylval.s_val = strdup(yytext); return SECTION; }
{SKIP}                      { yylval.s_val = strdup(yytext); return SKIP; } 
{WORD}                      { yylval.s_val = strdup(yytext); return WORD; }
{END}                       { yylval.s_val = strdup(yytext); return END; }
{GLOBAL}                    { yylval.s_val = strdup(yytext); return GLOBAL; }
{EXTERN}                    { yylval.s_val = strdup(yytext); return EXTERN; }

%r[0-9]|%r1[0-5]            { yylval.uint_val = (uint32_t) strtol(yytext + 2, NULL, 10); return REGISTER; }
0[xX][0-9a-fA-F]+|[0-9]+    { yylval.uint_val = (uint32_t) strtol(yytext, NULL, 0); return LITERAL; }
[a-zA-Z0-9]+                { yylval.s_val = strdup(yytext); return STRING; }
\n                          ;
[ \t]+                      ;
.                           { printf("Unknown character: %s\n", yytext); }

%%