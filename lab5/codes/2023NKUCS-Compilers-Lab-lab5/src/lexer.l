%option noyywrap
%option nounput
%option noinput
%option yylineno

%top{
    #include <stdarg.h>
    #include "common.h"
    #include <string>
    #include <fstream>
    #include "parser.h"
    using namespace std;
}
%{
    extern dump_type_t dump_type;

    inline void dump_tokens(const char* format, ...){
        va_list args;
        va_start(args, format);
        if (dump_type == TOKENS)
            vfprintf(yyout, format, args);
        va_end(args);
    }
%}

/* definitions section */
/*十进制整数*/
DECIMIAL ([1-9][0-9]*|0)
/*标识符*/
ID [[:alpha:]_][[:alpha:][:digit:]_]*

/*换行符*/
EOL (\r\n|\n|\r)
/*空白符*/
WHITE [\t ]
/*十六进制数*/
HEXNUM 0[xX][0-9A-Fa-f]+
/*八进制数*/
OCTNUM 0[1-7][0-7]*
/*浮点数*/
FLOATNUM (([0-9]+)\.[0-9]+)

DECIMIAL_FLOATNUM ((([0-9]*[.][0-9]*([eE][+-]?[0-9]+)?)|([0-9]+[eE][+-]?[0-9]+))[fLlL]?)
HEXNUM_FLOATNUM (0[xX](([0-9A-Fa-f]*[.][0-9A-Fa-f]*([pP][+-]?[0-9]+)?)|([0-9A-Fa-f]+[pP][+-]?[0-9]+))[fLlL]?)
OCTNUM_FLOATNUM (0(([0-7]*[.][0-7]*([pP][+-]?[0-9]+)?)|([0-7]+[pP][+-]?[0-9]+))[fLlL]?)

/*单行注释*/
SCOMMENT \/\/[^\n]*

%x MCOMMENT

%%
    /* rules section */

"," {
    dump_tokens("COMMA\t%s\n", yytext);
    return COMMA;
}

"const" {
    dump_tokens("IF\t%s\n", yytext);
    return CONST;
}

"int" {
    dump_tokens("INT\t%s\n", yytext);
    return INT;
}

"void" {
    dump_tokens("VOID\t%s\n", yytext);
    return VOID;
}

"float" {
    dump_tokens("FLOAT\t%s\n", yytext);
    return FLOAT;
}

"if" {
    dump_tokens("IF\t%s\n", yytext);
    return IF;
}

"else" {
    dump_tokens("ELSE\t%s\n", yytext);
    return ELSE;
}

"while" {
    dump_tokens("WHILE\t%s\n", yytext);
    return WHILE;
}

"break" {
    dump_tokens("BREAK\t%s\n", yytext);
    return BREAK;
}

"continue" {
    dump_tokens("CONTINUE\t%s\n", yytext);
    return CONTINUE;
}

"return" {
    dump_tokens("RETURN\t%s\n", yytext);
    return RETURN;
}

"==" {
    dump_tokens("EQUAL\t%s\n", yytext);
    return EQUAL;
}

"!=" {
    dump_tokens("NOTEQUAL\t%s\n", yytext);
    return NOTEQUAL;
}

"=" {
    dump_tokens("ASSIGN\t%s\n", yytext);
    return ASSIGN;
}

"!" {
    dump_tokens("NOT\t%s\n", yytext);
    return NOT;
}

"<=" {
    dump_tokens("LESSEQUAL\t%s\n", yytext);
    return LESSEQUAL;
}

"<" {
    dump_tokens("LESS\t%s\n", yytext);
    return LESS;
}

">=" {
    dump_tokens("GREATEREQUAL\t%s\n", yytext);
    return GREATEREQUAL;
}

">" {
    dump_tokens("GREATER\t%s\n", yytext);
    return GREATER;
}

"+" {
    dump_tokens("ADD\t%s\n", yytext);
    return ADD;
}

"-" {
    dump_tokens("SUB\t%s\n", yytext);
    return SUB;
}

"*" {
    dump_tokens("MUL\t%s\n", yytext);
    return MUL;
}

"/" {
    dump_tokens("DIV\t%s\n", yytext);
    return DIV;
}

"%" {
    dump_tokens("MOD\t%s\n", yytext);
    return MOD;
}

"&&" {
    dump_tokens("AND\t%s\n", yytext);
    return AND;
}

"||" {
    dump_tokens("OR\t%s\n", yytext);
    return OR;
}

";" {
    dump_tokens("SEMICOLON\t%s\n", yytext);
    return SEMICOLON;
}

"(" {
    dump_tokens("LPAREN\t%s\n", yytext);
    return LPAREN;
}

")" {
    dump_tokens("RPAREN\t%s\n", yytext);
    return RPAREN;
}

"[" {
    dump_tokens("LBRACKET\t%s\n", yytext);
    return LBRACKET;
}

"]" {
    dump_tokens("RBRACKET\t%s\n", yytext);
    return RBRACKET;
}

"{" {
    dump_tokens("LBRACE\t%s\n", yytext);
    return LBRACE;
}

"}" {
    dump_tokens("RBRACE\t%s\n", yytext);
    return RBRACE;
}

{ID} {
    yylval.strtype = strdup(yytext);
    dump_tokens("ID\t%s\n", yytext);
    //char *lexeme;
    // lexeme = new char[strlen(yytext) + 1];
    // strcpy(lexeme, yytext);
    return ID;
}

{DECIMIAL} {
    yylval.itype = atoi(yytext);
    dump_tokens("NUMBER\t%s\t%d\n", yytext, yylval.itype);
    return INTEGER;
}

{OCTNUM} {
    // string s(yytext);
    // int octnum = stoi(s, nullptr, 8);
    int octnum = 0;
    sscanf(yytext, "%o", &octnum);
    yylval.itype = octnum;
    dump_tokens("OCTNUM\t%s\t%d\n", yytext, octnum);
    return INTEGER;
}

{HEXNUM} {
    // string s(yytext);
    // int hexnum = stoi(s, nullptr, 16);
    int hexnum = 0;
    sscanf(yytext, "%x", &hexnum);
    yylval.itype = hexnum;
    dump_tokens("HEXNUM\t%s\t%d\n", yytext, hexnum);
    return INTEGER;
}

{FLOATNUM} {
    dump_tokens("FLOATNUM\t%s\t%f\n", yytext, atof(yytext));
    yylval.ftype = atof(yytext);
    return FLOATNUM;
}

{DECIMIAL_FLOATNUM} {
    dump_tokens("DECIMIAL_FLOATNUM\t%s\t%f\n", yytext, atof(yytext));
    // yylval.ftype = atof(yytext);
    sscanf(yytext, "%f", &yylval.ftype);
    return FLOATNUM;
}

{HEXNUM_FLOATNUM} {
    dump_tokens("HEXNUM_FLOATNUM\t%s\t%f\n", yytext, atof(yytext));
    // yylval.ftype = atof(yytext);
    sscanf(yytext, "%f", &yylval.ftype);
    return FLOATNUM;
}

{OCTNUM_FLOATNUM} {
    dump_tokens("OCTNUM_FLOATNUM\t%s\t%f\n", yytext, atof(yytext));
    // yylval.ftype = atof(yytext);
    sscanf(yytext, "%f", &yylval.ftype);
    return FLOATNUM;
}

{SCOMMENT} {
}

"/*" {
    BEGIN MCOMMENT;
}

<MCOMMENT>"*/" {
    BEGIN INITIAL;
}

<MCOMMENT>. {
}

<MCOMMENT>\r\n|\n|\r {
}

{EOL} { yylineno++;}

{WHITE} {
}

%%
/* user code section */
