%code top{
    #include <iostream>
    #include <assert.h>
    #include "parser.h"
    extern Ast ast;
    int yylex();
    int yyerror( char const * );
}

%code requires {
    #include "Ast.h"
    #include "SymbolTable.h"
    #include "Type.h"
}

%union {
    int itype;
    char* strtype;
    StmtNode* stmttype;
    ExprNode* exprtype;
    Type* type;
}

%start Program
%token <strtype> ID
%token <itype> INTEGER FLOATNUM
%token IF ELSE WHILE
%token INT VOID FLOAT
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON LBRACKET RBRACKET COMMA
%token ADD SUB MUL DIV MOD OR AND LESS LESSEQUAL GREATER GREATEREQUAL ASSIGN EQUAL NOTEQUAL NOT
%token CONST
%token RETURN BREAK CONTINUE

%nterm <stmttype> Stmts Stmt AssignStmt BlockStmt IfStmt WhileStmt BreakStmt ContinueStmt ReturnStmt DeclStmt FuncDef EmptyStmt FuncDefParams FuncDefParam VarDefs ConstDefs VarDef ConstDef ExprStmt MaybeFuncDefParams
%nterm <exprtype> Exp AddExp MulExp UnaryExp FuncCallExp Cond LOrExp EqExp PrimaryExp LVal RelExp LAndExp FuncCallParams
%nterm <type> Type

%precedence THEN
%precedence ELSE
%%
Program
    : Stmt {
        ast.setRoot($1);
    }
    ;
Type
    : INT {
        $$ = TypeSystem::intType;
    }
    | VOID {
        $$ = TypeSystem::voidType;
    }
    | FLOAT {
        $$ = TypeSystem::floatType;
    }
    ;
Stmts
    : Stmt {
        $$=$1;
    }
    | Stmts Stmt {
        $$ = new SeqNode($1, $2);
    }
    ;
Stmt
    : AssignStmt {$$=$1;}
    | BlockStmt {$$=$1;}
    | IfStmt {$$=$1;}
    | WhileStmt {$$=$1;}
    | BreakStmt {$$=$1;}
    | ContinueStmt {$$=$1;}
    | ReturnStmt {$$=$1;}
    | DeclStmt {$$=$1;}
    | FuncDef {$$=$1;}
    | EmptyStmt {$$=$1;}
    | ExprStmt {$$=$1;}
    ;
LVal
    : ID {
        SymbolEntry *se;
        se = identifiers->lookup($1);
        if(se == nullptr)
        {
            fprintf(stderr, "identifier \"%s\" is undefined\n", (char*)$1);
            delete [](char*)$1;
            assert(se != nullptr);
        }
        $$ = new Id(se);
        delete []$1;
    }
    ;
ExprStmt
    : Exp SEMICOLON {$$ = new ExprStmt($1);}
AssignStmt
    : LVal ASSIGN Exp SEMICOLON {$$ = new AssignStmt($1, $3);}
    ;
BlockStmt
    : LBRACE {
        identifiers = new SymbolTable(identifiers);
    }
    Stmts RBRACE {
        $$ = new CompoundStmt($3);
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
    }
    ;
IfStmt
    : IF LPAREN Cond RPAREN Stmt %prec THEN {$$ = new IfStmt($3, $5);}
    | IF LPAREN Cond RPAREN Stmt ELSE Stmt {$$ = new IfElseStmt($3, $5, $7);}
    ;
WhileStmt
    : WHILE LPAREN Cond RPAREN Stmt {$$ = new WhileStmt($3, $5);}
    ;
BreakStmt
    : BREAK SEMICOLON {$$ = new BreakStmt();}
    ;
ContinueStmt
    : CONTINUE SEMICOLON {$$ = new ContinueStmt();}
    ;
ReturnStmt
    : RETURN Exp SEMICOLON {$$ = new ReturnStmt($2);}
    ;
EmptyStmt
    : SEMICOLON {$$ = new EmptyStmt();}
    | LBRACE RBRACE {$$ = new EmptyStmt();}
    ;
Exp
    : AddExp {$$ = $1;}
    ;
Cond
    : LOrExp {$$ = $1;}
    ;
PrimaryExp
    : LPAREN Exp RPAREN {$$ = $2;}
    | LVal {$$ = $1;}
    | INTEGER {
        SymbolEntry* se = new ConstantSymbolEntry(TypeSystem::intType, $1);
        $$ = new Constant(se);
    }
    ;
AddExp
    : MulExp {$$ = $1;} // 乘法表达式
    | AddExp ADD MulExp { // 加法操作
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::ADD, $1, $3);
    }
    | AddExp SUB MulExp { // 减法操作
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::SUB, $1, $3);
    }
    ;
MulExp
    : UnaryExp {$$ = $1;} // 一元表达式
    | MulExp MUL UnaryExp { // 乘法操作
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::MUL, $1, $3);
    }
    | MulExp DIV UnaryExp { // 除法操作
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::DIV, $1, $3);
    }
    | MulExp MOD UnaryExp { // 求余操作
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::MOD, $1, $3);
    }
    ;
UnaryExp
    : PrimaryExp {$$ = $1;} // 主要表达式
    | ADD UnaryExp { // 一元加法
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new UnaryExpr(se, UnaryExpr::ADD, $2);
    }
    | SUB UnaryExp { // 一元减法
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new UnaryExpr(se, UnaryExpr::SUB, $2);
    }
    | NOT UnaryExp { // 逻辑 NOT
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new UnaryExpr(se, UnaryExpr::NOT, $2);
    }
    | FuncCallExp {$$ = $1;} // 函数调用表达式
    ;
RelExp
    : AddExp {$$ = $1;} // 加法表达式
    | RelExp LESS AddExp { // 小于操作
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LESS, $1, $3);
    }
    | RelExp LESSEQUAL AddExp { // 小于等于操作
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LESSEQUAL, $1, $3);
    }
    | RelExp GREATER AddExp { // 大于操作
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::GREATER, $1, $3);
    }
    | RelExp GREATEREQUAL AddExp { // 大于等于操作
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::GREATEREQUAL, $1, $3);
    }
    ;
EqExp
    : RelExp {$$ = $1;} // 关系表达式
    | EqExp EQUAL RelExp { // 等于操作
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::EQUAL, $1, $3);
    }
    | EqExp NOTEQUAL RelExp { // 不等于操作
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::NOTEQUAL, $1, $3);
    }
    ;
LAndExp
    : EqExp {$$ = $1;} // 相等表达式
    | LAndExp AND EqExp { // 逻辑与操作
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::AND, $1, $3);
    }
    ;
LOrExp
    : LAndExp {$$ = $1;} // 逻辑与表达式
    | LOrExp OR LAndExp { // 逻辑或操作
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::OR, $1, $3);
    }
    ;
DeclStmt
    : Type VarDefs SEMICOLON {$$ = $2;} // 变量声明
    | CONST Type ConstDefs SEMICOLON {$$ = $3;} // 常量声明
    ;
// 变量声明列表
VarDefs
    : VarDefs COMMA VarDef{
        $$ = $1;
        $1->setNext($3); // 参数通过指针连接
    }
    | VarDef {$$ = $1;}
    ;
// 变量声明
VarDef
    : ID {
        SymbolEntry* se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $1, identifiers->getLevel());
        identifiers->install($1, se);
        $$ = new DeclStmt(new Id(se));
        delete []$1;
    }
    | ID ASSIGN Exp {
        SymbolEntry* se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $1, identifiers->getLevel());
        identifiers->install($1, se);
        $$ = new DeclStmt(new Id(se), $3);
        delete []$1;
    }
    ;
// 常量声明列表
ConstDefs
    : ConstDefs COMMA ConstDef {
        $$ = $1;
        $1->setNext($3);
    }
    | ConstDef {$$ = $1;}
    ;
// 常量声明
ConstDef
    : ID ASSIGN Exp {
        SymbolEntry* se;
        se = new IdentifierSymbolEntry(TypeSystem::constIntType, $1, identifiers->getLevel());
        identifiers->install($1, se);
        $$ = new DeclStmt(new Id(se), $3);
        delete []$1;
    }
    ;

FuncDef
    :
    Type ID {
        Type *funcType;
        funcType = new FunctionType($1,{});
        SymbolEntry *se = new IdentifierSymbolEntry(funcType, $2, identifiers->getLevel());
        identifiers->install($2, se);
        identifiers = new SymbolTable(identifiers);
    }
    LPAREN RPAREN
    BlockStmt
    {
        SymbolEntry *se;
        se = identifiers->lookup($2);
        assert(se != nullptr);
        $$ = new FunctionDef(se, $6);
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
        delete []$2;
    }
    ;
%%

int yyerror(char const* message)
{
    std::cerr<<message<<std::endl;
    return -1;
}
