%{
/*********************************************
将所有的词法分析功能均放在 yylex 函数内实现，为 +、-、*、\、(、 ) 每个运算符及整数分别定义一个单词类别，在 yylex 内实现代码，能
识别这些单词，并将单词类别返回给词法分析程序。
实现功能更强的词法分析程序，可识别并忽略空格、制表符、回车等
空白符，能识别多位十进制整数。
YACC file
**********************************************/
#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include <string.h>

#ifndef YYSTYPE
#define YYSTYPE int
#endif
int yylex();
extern int yyparse();
FILE* yyin;
void yyerror(const char* s);

#ifndef NSYMS
#define NSYMS 30
#endif

struct symtable {
	char *name;
	int value;
}symtable[NSYMS] = {};

int lookup(char* s){
    for(int i = 0; i < NSYMS; i++) {
        if(symtable[i].name && !strcmp(s,symtable[i].name)) return i;
        if(!symtable[i].name) {
            symtable[i].name = strdup(s);
            return i;
        }
    }
    return -1;
}

%}


//TODO:给每个符号定义一个单词类别
%token ADD MINUS MUL DIV LPAREN RPAREN EQUAL ID NUMBER
%right EQUAL
%left ADD MINUS
%left MUL DIV
%right UMINUS

%%


lines   :       lines expr ';' { printf("%d\n", $2); }
        |       lines ID EQUAL expr ';'  { symtable[$2].value=$4; }
        |       lines ';'
        |
        ;
//TODO:完善表达式的规则
expr    :       expr ADD expr   { $$=$1+$3; }
        |       expr MINUS expr   { $$=$1-$3; }
        |       expr MUL expr { $$=$1*$3; }
        |       expr DIV expr { if($3 == 0) yyerror("divided by 0"); else $$=$1/$3; }
        |       MINUS expr %prec UMINUS   {$$=-$2;}
        |       LPAREN expr RPAREN { $$=$2; }
        |       NUMBER  { $$=$1; }
        |       ID  { $$=symtable[$1].value; }
        ;

%%

// programs section

int yylex()
{
    int t;
    while(1){
        t=getchar();
        if(t==' '||t=='\t'||t=='\n'){
            //do noting
        }else if(isdigit(t)){
            //TODO:解析多位数字返回数字类型 
            int tokenval = 0;
            while(isdigit(t)) {
                tokenval = tokenval * 10 + t - '0';
                t = getchar();
            }
            ungetc(t,stdin);
            yylval = tokenval;
            return NUMBER;
        }else if(t=='+'){
            return ADD;
        }else if(t=='-'){
            return MINUS;
        }//TODO:识别其他符号
        else if(t=='*'){
            return MUL;
        }else if(t=='/'){
            return DIV;
        }else if(t=='('){
            return LPAREN;
        }else if(t==')'){
            return RPAREN;
        }else if(t=='='){
            return EQUAL;
        }
        else if(isalpha(t) || t=='_'){
            char tokenval[100] = {};
            int curr = 0;
            while(isalpha(t) || isdigit(t) || t=='_'){
                tokenval[curr] = t;
                curr++;
                t = getchar();
            }
            ungetc(t,stdin);
            tokenval[curr] = '\n';
            int p = lookup(tokenval);
            if(p == -1) yyerror("too many symbol");
            yylval = p;
            return ID;
        }
        else{
            return t;
        }
    }
}

int main(void)
{
    yyin=stdin;
    do{
        yyparse();
    }while(!feof(yyin));
    return 0;
}
void yyerror(const char* s){
    fprintf(stderr,"Parse error: %s\n",s);
    exit(1);
}