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
#ifndef YYSTYPE
#define YYSTYPE int
#endif
int yylex();
extern int yyparse();
FILE* yyin;
void yyerror(const char* s);
%}

//TODO:给每个符号定义一个单词类别
%token ADD MINUS MUL DIV LPAREN RPAREN NUMBER
%left ADD MINUS
%left MUL DIV
%right UMINUS         

%%


lines   :       lines expr ';' {printf("pop {r1}\nldr r0, =a\nbl printf\nmov r0, #0\n");}
        |       lines ';'   
        |       
        ;
//TODO:完善表达式的规则
expr    :       expr ADD expr   { printf("pop {r0, r1}\nadd r0, r1, r0\npush {r0}\n"); }
        |       expr MINUS expr   { printf("pop {r0, r1}\nsub r0, r1, r0\npush {r0}\n"); }
        |       expr MUL expr { printf("pop {r0, r1}\nmul r0, r1, r0\npush {r0}\n"); }
        |       expr DIV expr { printf("pop {r1}\npop {r0}\nbl __aeabi_idiv\npush {r0}\n"); }
        |       MINUS expr %prec UMINUS   {printf("mov r0, #%d\npush {r0}\n",$2);}
        |       LPAREN expr RPAREN { $$=$2; }
        |       NUMBER  { printf("mov r0, #%d\npush {r0}\n",$1); }
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
        }else if(t == EOF){
            printf("pop {fp,pc}\n.section .note.GNU-stack,\"\",%%progbits\n");
            return t;
        }
        else{
            return t;
        }
    }
}

int main(void)
{
    printf(".arch armv7-a\n.align 2\n.section    .rodata\na:  .ascii \"num: %%d\\n\"\n.align 2\n.text\n.global main\nmain:\npush {fp,lr}\n");
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