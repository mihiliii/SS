%{  
    #include "bison.tab.h"
    #include "../inc/Assembler.hpp"
    #include "../inc/Instructions.hpp"
    
    extern int yyerror(const char* s);
    uint32_t line = 1;
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
CSRRD "csrrd"
CSRWR "csrwr"

/* Control status registers: */
STATUS "%status"
HANDLER "%handler"
CAUSE "%cause"

%%

"#"[^\n]*\n                 { line++; }
","                         { return ','; }
":"                         { return ':'; }
"$"                         { return '$'; }
{HALT}                      { return HALT; }
{INT}                       { return INT; }
{IRET}                      { return IRET; }
{CALL}                      { return CALL; }
{RET}                       { return RET; }
{JMP}                       { return JMP; }
{BEQ}                       { return BEQ; }
{BNE}                       { return BNE; }
{BGT}                       { return BGT; }
{PUSH}                      { return PUSH; }
{POP}                       { return POP; }
{XCHG}                      { return XCHG; }
{ADD}                       { return ADD; }
{SUB}                       { return SUB; }
{MUL}                       { return MUL; }
{DIV}                       { return DIV; }      
{NOT}                       { return NOT; }
{AND}                       { return AND; }
{OR}                        { return OR; }
{XOR}                       { return XOR; }
{SHL}                       { return SHL; }
{SHR}                       { return SHR; }
{LD}                        { return LD; }
{ST}                        { return ST; }
{CSRRD}                     { return CSRRD; }
{CSRWR}                     { return CSRWR; }

{SECTION}                   { return SECTION; }
{SKIP}                      { return SKIP; } 
{WORD}                      { return WORD; }
{END}                       { return END; }
{GLOBAL}                    { return GLOBAL; }
{EXTERN}                    { return EXTERN; }

{STATUS}                    { yylval.uint_val = (uint32_t) CSR::STATUS; return CSR; }
{HANDLER}                   { yylval.uint_val = (uint32_t) CSR::HANDLER; return CSR; }
{CAUSE}                     { yylval.uint_val = (uint32_t) CSR::CAUSE; return CSR; }

%r[0-9]|%r1[0-5]            { yylval.uint_val = (uint32_t) strtol(yytext + 2, NULL, 10); return GPR; }
0[xX][0-9a-fA-F]+|[0-9]+    { yylval.uint_val = (uint32_t) strtol(yytext, NULL, 0); return LITERAL; }
[a-zA-Z_][a-zA-Z0-9_]*      { yylval.s_val = strdup(yytext); return SYMBOL; }
\n                          { line++; return EOL; }
[ \t]+                      ;
.                           { printf("Syntax error, unknown character: %s\n", yytext); }

%%