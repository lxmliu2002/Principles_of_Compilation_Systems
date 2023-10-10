%{
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h> 
#include <string.h> 

#ifndef YYSTYPE
#define YYSTYPE int // 定义属性值类型为整数
#endif


int ti = 0; // 追踪字符串字符索引
int yylex();
extern int yyparse();
FILE *yyin;
void yyerror(const char *s);
int flag = 0;//是否第一个输入
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

%left ADD MINUS
%left MUL DIV
%right UMINUS
%left LEFTPAR RIGHTPAR

%%

lines	: lines expr ';'{
			printf("  pop {r1}\n");
			printf("  ldr r0, =str\n");
			printf("  bl printf\n");
			printf("  mov r0, #0\n"); 	
			printf("  pop {fp, pc}\n\n");
			printf(".extern printf\n");
			printf(".section .note.GNU-stack,\"\",%%progbits\n\n");
			printf("%d\n", $2); 
			flag = 0;
			} 
		| lines ';'
		|
		;

expr    : expr ADD expr {

			printf("  pop {r0, r1}\n");
			printf("  add r0, r0, r1\n");
			printf("  push {r0}\n");
			$$ = $1 + $3;
			}	
		| expr MINUS expr {
			printf("  pop {r0, r1}\n");
			printf("  sub r0, r0, r1\n");
			printf("  push {r0}\n");
			$$ = $1 - $3;
			}
		| expr MUL expr {
			printf("  pop {r0, r1}\n");
			printf("  mul r0, r0, r1\n");
			printf("  push {r0}\n");
			$$ = $1 * $3;
			}
		| expr DIV expr {
			printf("  pop {r1}\n");
			printf("  pop {r0}\n");
			printf("  bl __aeabi_idiv\n");
			printf("  push {r0}\n");
			$$ = $1 / $3;
			}
		| LEFTPAR expr RIGHTPAR {
			$$ = $2;
			}
		| UMINUS expr %prec UMINUS {
			printf("  pop r0, r1\n");
			printf("  mov r0, #0\n");
			printf("  sub r0, r0, r1\n");
			printf("  push {r0}\n");
			$$ = -$2;
			}
		| NUMBER {
			if(flag == 0)
			{
				printf("  .arch armv7-a\n");
				printf("  .data\n");
				printf("  .align 2\n");
				printf("str:\n");
				printf("  .ascii \" result:%%d\\n\"\n");
				printf("  .text\n");
				printf("  .align 2\n");
				printf("  .global main\n");
				printf("  .type main, %%function\n");
				printf("main:\n");
				printf("  push {fp, lr}\n");
			}
			printf("  mov r0, #%d\n", $1);
			printf("  push {r0}\n");
			flag = 1;
			$$ = $1;
			}
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
			yylval = 0;
			while (isdigit(t))	
			{
				yylval = yylval * 10 + t - '0';
				t = getchar();
			}
			ungetc(t, stdin); // 将字符 t 放回输入流
			isLastCharPram = 0;
			return NUMBER;
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

void yyerror(const char* s) {
	fprintf(stderr, "Parse error: %s\n", s);
	exit(1);
}
