%{  
    #include "bison.tab.h"
    
    extern int yyerror(const char* s);
    int line_number = 0;
%}
%option noyywrap

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

COMMA ","
HASHTAG "#"

%%

{COMMA}                  { return COMMA; }
{ADD}|{SUB}|{MUL}|{DIV}  { yylval.sval = strdup(yytext); return INSTRUCTION; }
%r[0-9]+                 { yylval.sval = strdup(yytext); return REGISTER; }
0[xX][0-9a-fA-F]+|[0-9]+ { yylval.ival = (int) strtol(yytext, NULL, 0); return NUMBER; }
[a-zA-Z0-9]+             { yylval.sval = strdup(yytext); return STRING; }
\n                       { line_number++; return EOL; }
[ \t]+                   ;
.                        { printf("Unknown character: %s\n", yytext); }

%%