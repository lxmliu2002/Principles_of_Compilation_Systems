%option noyywrap 
%option nounput
%option noinput  
%option yylineno

%top{
    #include <stdarg.h>
    #include "common.h"
    #include "symtab.h"
    #include <string>
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

    /* Your code here, if desired (lab3). */
    SymbolTable* symbolTable = new SymbolTable(); //符号表
    int column = 0; //列数
    int isDef = 0;
    int eqLeft = 1;
    int funcDef = 0;
    //yylineno = 0;

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
/*  Your code here (lab3). */
/*十六进制数*/
HEXNUM 0[xX][0-9A-Fa-f]+
/*八进制数*/
OCTNUM 0[1-7][0-7]*
/*浮点数*/
FLOATNUM (([0-9]+)\.[0-9]+)

DECIMIAL_FLOATNUM ((([0-9]*[.][0-9]*([eE][+-]?[0-9]+)?)|([0-9]+[eE][+-]?[0-9]+))[fLlL]?)
HEXNUM_FLOATNUM (0[xX](([0-9A-Fa-f]*[.][0-9A-Fa-f]*([pP][+-]?[0-9]+)?)|([0-9A-Fa-f]+[pP][+-]?[0-9]+))[fLlL]?)
OCTNUM_FLOATNUM (0(([0-7]*[.][0-7]*([pP][+-]?[0-9]+)?)|([0-7]+[pP][+-]?[0-9]+))[fLlL]?)

%x SCOMMENT
%x MCOMMENT


%%
    /* rules section */

"," {
    dump_tokens("COMMA\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
    eqLeft = 1;
}

"const" {
    dump_tokens("IF\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
}

"int" {
    dump_tokens("INT\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
    isDef = 1;

}

"void" {
    dump_tokens("VOID\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
    isDef = 1;

}

"float" {
    dump_tokens("FLOAT\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
    isDef = 1;

}

"if" {
    dump_tokens("IF\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
}

"else" {
    dump_tokens("ELSE\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
}

"return" {
    dump_tokens("RETURN\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
}

"while" {
    dump_tokens("WHILE\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
}

"continue" {
    dump_tokens("CONTINUE\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
}

"break" {
    dump_tokens("BREAK\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
}

"==" {
    dump_tokens("EQUAL\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
}

"=" {
    dump_tokens("ASSIGN\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
    eqLeft = 0;
}

"!=" {
    dump_tokens("NOTEQUAL\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
}

"!" {
    dump_tokens("NOT\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
}

"<=" {
    dump_tokens("LESSEQUAL\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
}

"<" {
    dump_tokens("LESS\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
}

">=" {
    dump_tokens("GREATEREQUAL\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
}

">" {
    dump_tokens("GREATER\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
}

"+" {
    dump_tokens("ADD\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
}

"-" {
    dump_tokens("MINUS\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
}

"*" {
    dump_tokens("MUL\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
}

"/" {
    dump_tokens("DIV\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
}

"%" {
    dump_tokens("MOD\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
}

"&&" {
    dump_tokens("AND\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
}

"||" {
    dump_tokens("OR\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
}

";" {
    dump_tokens("SEMICOLON\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
    isDef = 0;
    eqLeft = 1;
    BEGIN INITIAL;
}

"(" {
    dump_tokens("LPAREN\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
}

")" {
    dump_tokens("RPAREN\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
}

"[" {
    dump_tokens("LBRACKET\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
}

"]" {
    dump_tokens("RBRACKET\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
}

"{" {
    dump_tokens("LBRACE\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
    if(!funcDef)
    {
        Scope* newScope = new Scope;
        symbolTable->addChild(symbolTable->curr, newScope);
        symbolTable->curr = newScope;
    }
    funcDef = 0;
}

"}" {
    dump_tokens("RBRACE\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
    symbolTable->retParent();
}

{DECIMIAL} {
    int decimal;
    decimal = atoi(yytext);
    dump_tokens("DECIMAL\t%s\t%d\t%d\t%d\n", yytext, yylineno - 1, column, decimal);
    column += yyleng;
}

{OCTNUM} {
    int octnum = 0;
    sscanf(yytext, "%o", &octnum);
    dump_tokens("OCTNUM\t%s\t%d\t%d\n", yytext, yylineno - 1, column, octnum);
	column += yyleng;
}

{HEXNUM} {
    int hexnum = 0;
    sscanf(yytext, "%x", &hexnum);
    dump_tokens("HEXNUM\t%s\t%d\t%d\n", yytext, yylineno - 1, column, hexnum);
	column += yyleng;
}

{FLOATNUM} {
    dump_tokens("FLOATNUM\t%s\t%d\t%d\t%f\n", yytext, yylineno - 1, column, atof(yytext));
	column += yyleng;
}

{DECIMIAL_FLOATNUM} {
    dump_tokens("DECIMAL_FLOATNUM\t%s\t%d\t%d\t%f\n", yytext, yylineno - 1, column, atof(yytext));
    column += yyleng;
}

{HEXNUM_FLOATNUM} {
    dump_tokens("HEXNUM_FLOATNUM\t%s\t%d\t%d\t%f\n", yytext, yylineno - 1, column, atof(yytext));
    column += yyleng;
}

{OCTNUM_FLOATNUM} {
    dump_tokens("OCTNUM_FLOATNUM\t%s\t%d\t%d\t%f\n", yytext, yylineno - 1, column, atof(yytext));
    column += yyleng;
}

{ID} {
    //char* str = yytext;
    Scope* temp = symbolTable->findScope(yytext, symbolTable->curr);
    Scope* sc = nullptr;
    // printf("%s, %d, %d\n",yytext, isDef, eqLeft);
    if(temp == nullptr || (isDef && eqLeft))
    {
        sc = symbolTable->curr;
        Symbol* sym = new Symbol;
        sym->name = strdup(yytext);
        symbolTable->addSymbol(sym, sc);
    }
    else
    {
        sc = temp;
    }
    dump_tokens("ID\t%s\t%d\t%d\t%p\n", yytext, yylineno - 1, column, symbolTable->getSymbol(yytext, sc));
	column += yyleng;
}

"//" {
    dump_tokens("SCOMMENT\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
    BEGIN SCOMMENT;
}

<SCOMMENT>.* {
    dump_tokens("COMMENT\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
}

<SCOMMENT>\n {
    column = 0;
    BEGIN INITIAL;
}

"/*" {
    dump_tokens("MCOMMENTBEGIN\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
    BEGIN MCOMMENT;
}

<MCOMMENT>"*/" {
    dump_tokens("MCOMMENTEND\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
    BEGIN INITIAL;
}

<MCOMMENT>. {
    dump_tokens("MCOMMENT\t%s\t%d\t%d\n", yytext, yylineno - 1, column);
	column += yyleng;
}

<MCOMMENT>\r\n|\n|\r {
    column = 0;
}

{EOL} { column=0;}

{WHITE} {
    column += yyleng;
}


%%
/* user code section */
