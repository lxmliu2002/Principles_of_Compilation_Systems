%{
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h> 
#include <string.h> 

#ifndef YYSTYPE
#define YYSTYPE char*  // 定义属性值类型
#endif

char idStr[50];
char numStr[50];
int yylex();
extern int yyparse();
FILE *yyin;
void yyerror(const char *s);
%}

// %token 为每个符号分配不同的整数值
%token ADD      
%token MINUS
%token MUL
%token DIV
%token LEFTPAR
%token RIGHTPAR
%token NUMBER // 数字对应属性值，以字符串形式存储
%token UMINUS // 取相反数
%token ID  // 标识符对应属性值

%left ADD MINUS
%left MUL DIV
%right UMINUS
%left LEFTPAR RIGHTPAR

%%

// 语法规则部分
lines	: lines expr ';'	{ printf("%s\n", $2); }  // 打印后缀表达式
		| lines ';'  // 空行
		|
		;

expr    : expr ADD expr { $$ = (char *)malloc(50*sizeof(char)); strcpy($$, $1); strcat($$, $3); strcat($$, "+"); }
		| expr MINUS expr { $$ = (char *)malloc(50*sizeof(char)); strcpy($$, $1); strcat($$, $3); strcat($$, "-"); }
		| expr MUL expr { $$ = (char *)malloc(50*sizeof(char)); strcpy($$, $1); strcat($$, $3); strcat($$, "*"); }
		| expr DIV expr { $$ = (char *)malloc(50*sizeof(char)); strcpy($$, $1); strcat($$, $3); strcat($$, "/"); }
		| LEFTPAR expr RIGHTPAR { $$ = (char *)malloc(50*sizeof(char)); strcpy($$, $2); }  // 括号表达式
		| UMINUS expr %prec UMINUS { $$ = (char *)malloc(50*sizeof(char)); strcpy($$, $2); strcat($$, " UMINUS"); }  // 负数表达式
		| NUMBER { $$ = (char *)malloc(50*sizeof(char)); strcpy($$, $1); }  // 数字表达式
		| ID { $$ = (char *)malloc(50*sizeof(char)); strcpy($$, $1); strcat($$, "");}  // 变量引用表达式
		;
%%

int isLastCharPram = 0; //表示上一个字符是否是操作符
int count = 0; //计算读入的字符的数量

int yylex()
{
	int t = 0;
	count++;
	while(1)
	{
		t = getchar();
		if(t == ' ' || t == '\t' || t == '\n')
		{
			;
		}
		else if(t >= '0' && t <= '9')
		{
			int ti = 0;
			while (isdigit(t))	
			{
				numStr[ti] = t;
				t = getchar();
				ti++;
			}
			numStr[ti] = '\0';
			ungetc(t, stdin); // 将字符 t 放回输入流
			yylval = numStr;
			isLastCharPram = 0;
			return NUMBER;
		}
		else if((t >= 'a' && t <= 'z') || (t >= 'A' && t <= 'Z' ) || (t == '_'))
		{
			int ti = 0;
			while ((t >= 'a' && t <= 'z') || (t >= 'A' && t <= 'Z') || ( t == '_' ) || (t >= '0' && t <= '9'))
			{
				idStr[ti] = t;
				t = getchar();
				ti++;
			}
			idStr[ti] = '\0'; // 字符串结尾
			yylval = idStr;
			ungetc(t, stdin);
			isLastCharPram = 0;
			return ID;
		}
		else if (t == '-')
		{
			if(count != 1 && isLastCharPram == 0)
			{
				return MINUS;
			}
			else
			{
				isLastCharPram = 0;
				return UMINUS;
			}
		} 
		else if (t == '(')
		{
			isLastCharPram = 1;
			return LEFTPAR;
		}
		else if(t == ';')
		{
			count = 0;
			return t;
		}
		else if(t == '+')
		{
			isLastCharPram = 0;
			return ADD;
		}
		else if(t == '*')
		{
			isLastCharPram = 0;
			return MUL;
		}
		else if (t == '/') 
		{
			isLastCharPram = 0;
			return DIV;
		}
		else if (t == ')') 
		{
			isLastCharPram = 0;
			return RIGHTPAR;
		}
		else 
		{
			return t;
		}
	}
}


int main()
{
	yyin = stdin;
	do {
		yyparse();
	} while (!feof(yyin));
	return 0;
}

// 语法分析错误处理
void yyerror(const char* s) {
	fprintf(stderr, "Parse error: %s\n", s);
	exit(1);
}
