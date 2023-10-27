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

%nterm <stmttype> Stmts Stmt AssignStmt BlockStmt IfStmt WhileStmt BreakStmt ContinueStmt ReturnStmt DeclStmt FuncDef EmptyStmt FuncDefParams FuncDefParam VarDefs ConstDefs VarDef ConstDef ExprStmt funcBlock
%nterm <exprtype> Exp AddExp FuncCallExp Cond PrimaryExp LVal ExpList
%nterm <type> Type

%left OR
%left AND
%left EQUAL NOTEQUAL
%left LESS LESSEQUAL GREATER GREATEREQUAL
%left ADD SUB
%left MUL DIV MOD
%precedence UMINUS   

%precedence THEN
%precedence ELSE
%%
Program
    : Stmts {
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
    : AddExp {$$ = $1;}
    ;
PrimaryExp
    : LVal {$$ = $1;}
    | INTEGER {
        SymbolEntry* se = new ConstantSymbolEntry(TypeSystem::intType, $1);
        $$ = new Constant(se);
    }
    | FLOATNUM {
        SymbolEntry* se = new ConstantSymbolEntry(TypeSystem::floatType, $1);
        $$ = new Constant(se);
    }
    | FuncCallExp { $$ = $1; }
    ;

AddExp
    : AddExp ADD AddExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::ADD, $1, $3);
    }
    | AddExp SUB AddExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::SUB, $1, $3);
    }
    | AddExp MUL AddExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::MUL, $1, $3);
    }
    | AddExp DIV AddExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::DIV, $1, $3);
    }
    | AddExp MOD AddExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::MOD, $1, $3);
    }
    | AddExp LESS AddExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LESS, $1, $3);
    }
    | AddExp LESSEQUAL AddExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LESSEQUAL, $1, $3);
    }
    | AddExp GREATER AddExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::GREATER, $1, $3);
    }
    | AddExp GREATEREQUAL AddExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::GREATEREQUAL, $1, $3);
    }
    | AddExp EQUAL AddExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::EQUAL, $1, $3);
    }
    | AddExp NOTEQUAL AddExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::NOTEQUAL, $1, $3);
    }
    | AddExp AND AddExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::AND, $1, $3);
    }
    | AddExp OR AddExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::OR, $1, $3);
    }
    | ADD AddExp %prec UMINUS {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new UnaryExpr(se, UnaryExpr::ADD, $2);
    }
    | SUB AddExp %prec UMINUS {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new UnaryExpr(se, UnaryExpr::SUB, $2);
    }
    | NOT AddExp %prec UMINUS {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new UnaryExpr(se, UnaryExpr::NOT, $2);
    }
    | LPAREN AddExp RPAREN {$$ = $2;}
    | PrimaryExp { $$ = $1; }
    ;

// binaryOp
//     : ADD { $$ = BinaryExpr::ADD; }
//     | SUB { $$ = BinaryExpr::SUB; }
//     | MUL { $$ = BinaryExpr::MUL; }
//     | DIV { $$ = BinaryExpr::DIV; }
//     | MOD { $$ = BinaryExpr::MOD; }
//     | LESS { $$ = BinaryExpr::LESS; }
//     | LESSEQUAL { $$ = BinaryExpr::LESSEQUAL; }
//     | GREATER { $$ = BinaryExpr::GREATER; }
//     | GREATEREQUAL { $$ = BinaryExpr::GREATEREQUAL; }
//     | EQUAL { $$ = BinaryExpr::EQUAL; }
//     | NOTEQUAL { $$ = BinaryExpr::NOTEQUAL; }
//     | AND { $$ = BinaryExpr::AND; }
//     | OR { $$ = BinaryExpr::OR; }
//     ;

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
    Type ID LPAREN {
        Type *funcType;
        funcType = new FunctionType($1,vector<Type*>());
        SymbolEntry *se = new IdentifierSymbolEntry(funcType, $2, identifiers->getLevel());
        identifiers->install($2, se);
        identifiers = new SymbolTable(identifiers);
    }
    FuncDefParams RPAREN funcBlock {
        vector<Type*> paramsType;
        DeclStmt* curr = (DeclStmt*)$5;
        while(curr != nullptr)
        {
            paramsType.push_back(curr->getId()->getSymbolEntry()->getType());
            curr = (DeclStmt*)(curr->getNext());
        }
        SymbolEntry *se;
        se = identifiers->lookup($2);
        assert(se != nullptr);
        FunctionType* tmp = (FunctionType*)(se->getType());
        tmp->setParamsType(paramsType);
        $$ = new FunctionDef(se,new CompoundStmt($7), (DeclStmt*)$5);
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
        delete []$2;
    }
    ;

funcBlock 
    : LBRACE Stmts RBRACE {
        $$ = $2;
    }
    | LBRACE RBRACE {
        $$ = new EmptyStmt();
    }

FuncDefParams
    : %empty { $$ = nullptr; }
    | FuncDefParam {
        $$ = $1;
    }
    | FuncDefParams COMMA FuncDefParam {
        $$ = $1;
        $$->setNext($3);
    }

FuncDefParam
    : Type ID {
        SymbolEntry* se;
        se = new IdentifierSymbolEntry($1, $2, identifiers->getLevel());
        identifiers->install($1->toStr(), se);
        $$ = new DeclStmt(new Id(se));
        delete []$2;
    }

FuncCallExp
    : ID LPAREN ExpList RPAREN {
        SymbolEntry *se;
        se = identifiers->lookup($1);
        assert(se != nullptr);
        $$ = new FuncCallExp(se, $3);
    }
    ;

ExpList
    : %empty { $$ = nullptr; }
    | AddExp {
        $$ = $1;
    }
    | ExpList COMMA AddExp {
        $$ = $1;
        $$->setNext($3);
    }
    ;

%%

int yyerror(char const* message)
{
    std::cerr<<message<<std::endl;
    return -1;
}
