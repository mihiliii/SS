%{
    #include "snazzle.tab.h"
    extern int line_num;
%}
%option noyywrap
%%
[0-9]+\.[0-9]+ { yylval.fval = atof(yytext); return FLOAT; }
[0-9]+         { yylval.ival = atoi(yytext); return INT; }
[a-zA-Z0-9]+   { yylval.sval = strdup(yytext); return STRING; }
\n             { line_num++; }
[ \t]          ;
.              ;
%%