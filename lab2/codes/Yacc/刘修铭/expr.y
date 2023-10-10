%{
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h> 
#include <string.h> 

#define NSYMS 20        /* 最大符号数量 */

struct symtab {
        char *name;
        double value;
} symtab[NSYMS];       // 记录标识符名称和值的符号表
struct symtab *symlook(char *s); // 用于符号表管理，查找或添加标识符到符号表中
char idStr[50];  // 存储标识符的字符串
int yylex();  // 定义词法分析器
extern int yyparse();  // 语法分析器
FILE* yyin;  // 处理输入文件
void yyerror(const char* s);  // 报告语法错误
%}

// 属性值可能具有的类型
%union {
        double  dval;
        struct symtab *symp;
}

// %token 为每个符号分配不同的整数值
%token ADD      
%token MINUS
%token MUL
%token DIV
%token LEFTPAR
%token RIGHTPAR
%token <dval> NUMBER // 数字对应属性值
%token UMINUS // 取相反数
%token equal
%token <symp> ID  // 标识符对应属性值

%right equal
%left ADD MINUS
%left MUL DIV
%right UMINUS
%left LEFTPAR RIGHTPAR

// 表达式的属性值设置为数值类型
%type <dval> expr

%%

// 语法规则部分
lines	: lines expr ';'	{ printf("%f\n", $2); }  // 打印表达式结果
		| lines ';'  // 空行
		|
		;

expr	: expr ADD expr { $$ = $1 + $3; }  // 加法表达式
		| expr MINUS expr { $$ = $1 - $3; }  // 减法表达式
		| expr MUL expr { $$ = $1 * $3; }  // 乘法表达式
		| expr DIV expr { $$ = $1 / $3; }  // 除法表达式
		| LEFTPAR expr RIGHTPAR { $$ = $2; }  // 括号表达式
		| UMINUS expr %prec UMINUS { $$ = -$2; }  // 负数表达式 %prec UMINUS 用于指定这个产生式的优先级
		| NUMBER { $$ = $1; }  // 数字表达式
		| ID equal expr { $1->value=$3; $$=$3;}  // 赋值表达式
		| ID {$$=$1->value;}  // 变量引用表达式
		;


%%

int isLastCharPram = 0; //表示上一个字符是否是操作符
int count = 0; //计算读入的字符的数量

// 词法分析器
int yylex()
{
	int t = 0;
	count++;
	while (1)
	{
		t = getchar();
		if (t == ' ' || t == '\n' || t == '\t') { ; }  // 忽略空白字符
		else if (isdigit(t))  // 数字
		{
			yylval.dval = 0;  // ylval 是一个特殊的变量，它被定义为一个与词法规则相关联的联合体（union）
			while (isdigit(t))
			{
				yylval.dval = yylval.dval * 10 + t - '0';
				t = getchar();
			}
			ungetc(t, stdin); // 将字符 t 放回输入流,以便后续的输入函数可以再次读取
			isLastCharPram = 0;
			return NUMBER;
		}
		else if ((t>='a'&&t<='z')||(t>='A'&&t<='Z')||(t=='_'))  // 此处考虑标识符，而标识符通常以字母或下划线开头
		{
			int ti = 0;
			while ((t >= 'a' && t <= 'z') || (t >= 'A' && t <= 'Z') || ( t == '_' ) || (t >= '0' && t <= '9'))
			{
				idStr[ti]=t;
				t = getchar();
				ti++;
			}
			idStr[ti] = '\0'; // 字符串结尾

			char* temp = (char*) malloc (50*sizeof(char)); 
			strcpy(temp,idStr);
			yylval.symp = symlook(temp); 

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
			count = 0;   // 每次读入一个字符串重新清零
			return t;
		}
        else if (t == '=')
		{
			isLastCharPram = 0;
			return equal;
		}
		else
		{
			isLastCharPram = 0;
			if (t == '+') 
            {
                return ADD;
            }
			else if (t == '*') 
            {
                return MUL;
            }
            else if (t == '/') 
            {
                return DIV;
            }
			else if (t == ')') 
            {
                return RIGHTPAR;
            }
			else 
            {
                return t;
            }
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


// 符号表查找程序，返回一个符号表的表项
struct symtab* symlook(char *s){
    char *p;
    struct symtab *sp;
	// 遍历符号表中的每一项
    for(sp = symtab ; sp < &symtab[NSYMS] ; sp ++)
    {
		// 如果找到了表中存在这一项，返回指向该表项的指针
        if(sp -> name && !strcmp(sp -> name, s))
            return sp;
		// 如果找不到这一项，那就新建一个表项，strdup函数用于拷贝字符串
        if(!sp -> name){
            sp -> name = strdup(s);
            return sp;
        }
    }
    yyerror("Wrong input");  // 如果符号表已满，报错
    exit(1);
}
