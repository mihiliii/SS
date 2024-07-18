
%{
    #include <cstdio>
    #include <iostream>
    using namespace std;

    // Declare stuff from Flex that Bison needs to know about:
    extern int yylex();
    extern int yyparse();
    extern FILE *yyin;
 
    void yyerror(const char *s);
%}

// Bison fundamentally works by asking flex to get the next token, which it
// returns as an object of type "yystype".  Initially (by default), yystype
// is merely a typedef of "int", but for non-trivial projects, tokens could
// be of any arbitrary data type.  So, to deal with that, the idea is to
// override yystype's default typedef to be a C union instead.  Unions can
// hold all of the types of tokens that Flex could return, and this this means
// we can return ints or floats or strings cleanly.  Bison implements this
// mechanism with the %union directive:
%union {
    int ival;
    char* sval;
}

// Define the "terminal symbol" token types I'm going to use (in CAPS
// by convention), and associate each with a field of the %union:
%token <ival> NUMBER
%token <sval> STRING

%token <sval> INSTRUCTION
%token <sval> REGISTER

%token EOL
%token COMMA

%%
// This is the actual grammar that bison will parse, but for right now it's just
// something silly to echo to the screen what bison gets from flex. We'll
// make a real one shortly:
input:
    | line input ;

line:
    INSTRUCTION REGISTER COMMA REGISTER EOL
    {
        cout << $1 << " " << $2 << " " << $4 << endl;
    };

%%

void yyerror(const char *s) {
    cout << "Parse error! Message: " << s << endl;
    // might as well halt now:
    exit(-1);
}
