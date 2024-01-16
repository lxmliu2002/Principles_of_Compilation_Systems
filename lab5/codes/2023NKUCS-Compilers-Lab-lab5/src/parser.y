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
    float ftype;
    char* strtype;
    StmtNode* stmttype;
    ExprNode* exprtype;
    Type* type;
}

%start Program
%token <strtype> ID
%token <itype> INTEGER 
%token <ftype> FLOATNUM
%token IF ELSE WHILE
%token INT VOID FLOAT
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON LBRACKET RBRACKET COMMA
%token ADD SUB MUL DIV MOD OR AND LESS LESSEQUAL GREATER GREATEREQUAL ASSIGN EQUAL NOTEQUAL NOT
%token CONST
%token RETURN BREAK CONTINUE

%nterm <stmttype> Stmts Stmt AssignStmt BlockStmt IfStmt WhileStmt BreakStmt ContinueStmt ReturnStmt DeclStmt FuncDef EmptyStmt FuncDefParams FuncDefParam VarDefs VarDef ExprStmt funcBlock
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
        $$=new SeqNode();
        ((SeqNode*)$$)->addStmtBack($1);
    }
    | Stmts Stmt {
        $$ = $1;
        ((SeqNode*)$$)->addStmtBack($2);
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
        if(se == nullptr){
            fprintf(stderr, "identifier \"%s\" is undefined\n", (char*)$1);
            delete [](char*)$1;
            assert(se != nullptr);
        }
        $$ = new Id(se);
        delete []$1;
    }
    // | ID ArrDef {
    //     SymbolEntry *se;
    //     se = identifiers->lookup($1);
    //     if(se == nullptr){
    //         fprintf(stderr, "identifier \"%s\" is undefined\n", (char*)$1);
    //         delete [](char*)$1;
    //         assert(se != nullptr);
    //     }
    //     $$ = new Id(se, $2);
    //     delete []$1;
    // }
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
    | RETURN SEMICOLON {$$ = new ReturnStmt(nullptr);}
    ;
EmptyStmt
    : SEMICOLON {$$ = new EmptyStmt();}
    | LBRACE RBRACE {$$ = new EmptyStmt();}
    ;
Exp
    : AddExp {$$ = $1;}
    ;
// ConstExp
//     : AddExp {$$ = $1;}
//     ;
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
        Type* type;
        if($1->getSymPtr()->getType() == TypeSystem::floatType 
        || $3->getSymPtr()->getType() == TypeSystem::floatType )
            type = TypeSystem::floatType;
        else 
            type = TypeSystem::intType;
        SymbolEntry *se = new TemporarySymbolEntry(type, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::ADD, $1, $3);
    }
    | AddExp SUB AddExp {
        Type* type;
        if($1->getSymPtr()->getType() == TypeSystem::floatType 
        || $3->getSymPtr()->getType() == TypeSystem::floatType )
            type = TypeSystem::floatType;
        else 
            type = TypeSystem::intType;
        SymbolEntry *se = new TemporarySymbolEntry(type, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::SUB, $1, $3);
    }
    | AddExp MUL AddExp {
        Type* type;
        if($1->getSymPtr()->getType() == TypeSystem::floatType 
        || $3->getSymPtr()->getType() == TypeSystem::floatType )
            type = TypeSystem::floatType;
        else 
            type = TypeSystem::intType;
        SymbolEntry *se = new TemporarySymbolEntry(type, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::MUL, $1, $3);
    }
    | AddExp DIV AddExp {
        Type* type;
        if($1->getSymPtr()->getType() == TypeSystem::floatType 
        || $3->getSymPtr()->getType() == TypeSystem::floatType )
            type = TypeSystem::floatType;
        else 
            type = TypeSystem::intType;
        SymbolEntry *se = new TemporarySymbolEntry(type, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::DIV, $1, $3);
    }
    | AddExp MOD AddExp {
        Type* type;
        if($1->getSymPtr()->getType() == TypeSystem::floatType 
        || $3->getSymPtr()->getType() == TypeSystem::floatType )
            type = TypeSystem::floatType;
        else 
            type = TypeSystem::intType;
        SymbolEntry *se = new TemporarySymbolEntry(type, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::MOD, $1, $3);
    }
    | AddExp LESS AddExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LESS, $1, $3);
    }
    | AddExp LESSEQUAL AddExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LESSEQUAL, $1, $3);
    }
    | AddExp GREATER AddExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::GREATER, $1, $3);
    }
    | AddExp GREATEREQUAL AddExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::GREATEREQUAL, $1, $3);
    }
    | AddExp EQUAL AddExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::EQUAL, $1, $3);
    }
    | AddExp NOTEQUAL AddExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::NOTEQUAL, $1, $3);
    }
    | AddExp AND AddExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::AND, $1, $3);
    }
    | AddExp OR AddExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::OR, $1, $3);
    }
    | ADD AddExp %prec UMINUS {
        Type* type;
        if($2->getSymPtr()->getType() == TypeSystem::floatType )
            type = TypeSystem::floatType;
        else 
            type = TypeSystem::intType;
        SymbolEntry *se = new TemporarySymbolEntry(type, SymbolTable::getLabel());
        $$ = new UnaryExpr(se, UnaryExpr::ADD, $2);
    }
    | SUB AddExp %prec UMINUS {
        if($2->getSymPtr()->isConstant())
        {
            ConstantSymbolEntry* tmp = (ConstantSymbolEntry*)$2->getSymPtr();
            if(tmp->getType()->isInt())
            {
                tmp->setValue(-tmp->getInt());
            }
            else if(tmp->getType()->isFloat())
            {
                tmp->setValue(-tmp->getFloat());
            }
            $$ = $2;
        }
        else
        {
            Type* type;
            if($2->getSymPtr()->getType() == TypeSystem::floatType )
                type = TypeSystem::floatType;
            else 
                type = TypeSystem::intType;
            SymbolEntry *se = new TemporarySymbolEntry(type, SymbolTable::getLabel());
            $$ = new UnaryExpr(se, UnaryExpr::SUB, $2);
        }
    }
    | NOT AddExp %prec UMINUS {
        Type* type;
        type = TypeSystem::boolType;
        SymbolEntry *se = new TemporarySymbolEntry(type, SymbolTable::getLabel());
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
    : Type VarDefs SEMICOLON {
        $$ = $2;
        DeclStmt* curr = (DeclStmt*)$$;
        while(curr != nullptr)
        {
            curr->getId()->getSymPtr()->setType($1);
            curr = (DeclStmt*)curr->getNext();
        }
    }
    | CONST Type VarDefs SEMICOLON {
        $$ = $3;
        DeclStmt* curr = (DeclStmt*)$$;
        while(curr != nullptr)
        {
            curr->getId()->getSymPtr()->setType($2);
            ((IdentifierSymbolEntry*)curr->getId()->getSymPtr())->setConst();
            curr = (DeclStmt*)curr->getNext();
        }
    }
    ;
VarDefs
    : VarDefs COMMA VarDef{
        $$ = $1;
        $1->setNext($3);
    }
    | VarDef {$$ = $1;}
    ;
VarDef
    : ID {
        // Type *type;
        // if($1->getSymPtr()->getType() == TypeSystem::floatType)
        //     type = TypeSystem::floatType;
        // else 
        //     type = TypeSystem::intType;
        // SymbolEntry* se = new IdentifierSymbolEntry(type, $1, identifiers->getLevel());
        if(identifiers->checkExist($1))
        {
            fprintf(stderr, "redefined variable %s\n", $1);
            exit(-1);
        }
        SymbolEntry* se = new IdentifierSymbolEntry(TypeSystem::intType, $1, identifiers->getLevel());
        identifiers->install($1, se);
        $$ = new DeclStmt(new Id(se));
        delete []$1;
    }
    | ID ASSIGN Exp {
        // Type *type;
        // if($3->getSymPtr()->getType() == TypeSystem::floatType)
        //     type = TypeSystem::floatType;
        // else 
        //     type = TypeSystem::intType;
        // SymbolEntry* se = new IdentifierSymbolEntry(type, $1, identifiers->getLevel());
        if(identifiers->checkExist($1))
        {
            fprintf(stderr, "redefined variable %s\n", $1);
            exit(-1);
        }
        SymbolEntry* se = new IdentifierSymbolEntry(TypeSystem::intType, $1, identifiers->getLevel());
        identifiers->install($1, se);
        $$ = new DeclStmt(new Id(se), $3);
        delete []$1;
    }
    // TODO
    // | ID ArrDef {

    // }
    // | ID ArrDef ASSIGN {

    // }
    ;
// ConstDefs
//     : ConstDefs COMMA ConstDef {
//         $$ = $1;
//         $1->setNext($3);
//     }
//     | ConstDef {$$ = $1;}
//     ;
// ConstDef
//     : ID ASSIGN Exp {
//         // Type *type;
//         // if($1->getSymPtr()->getType() == TypeSystem::floatType)
//         //     type = TypeSystem::floatType;
//         // else 
//         //     type = TypeSystem::intType;
//         // SymbolEntry* se = new IdentifierSymbolEntry(type, $1, identifiers->getLevel());
//         SymbolEntry* se = new IdentifierSymbolEntry(intType, $1, identifiers->getLevel());
//         identifiers->install($1, se);
//         $$ = new DeclStmt(new Id(se), $3);
//         delete []$1;
//     }
    // | ID ArrDef ASSIGN {

    // }
    ;
FuncDef
    :
    Type ID LPAREN {
        Type *funcType;
        funcType = new FunctionType($1,vector<Type*>());
        if(identifiers->checkExist($2))
        {
            fprintf(stderr, "redefined function\n");
            exit(-1);
        }
        SymbolEntry *se = new IdentifierSymbolEntry(funcType, $2, identifiers->getLevel());
        identifiers->install($2, se);
        identifiers = new SymbolTable(identifiers);
    }
    FuncDefParams RPAREN {
        identifiers = new SymbolTable(identifiers);
        DeclStmt* curr = (DeclStmt*)$5;
        while(curr != nullptr)
        {
            std::string name = ((IdentifierSymbolEntry*)curr->getId()->getSymPtr())->getName();
            SymbolEntry *se = new IdentifierSymbolEntry(curr->getId()->getSymPtr()->getType(), name, identifiers->getLevel());
            identifiers->install(name, se);
            curr = (DeclStmt*)(curr->getNext());
        }
    }
    funcBlock {
        vector<Type*> paramsType;
        DeclStmt* curr = (DeclStmt*)$5;
        DeclStmt* paramDecl = nullptr;
        while(curr != nullptr)
        {
            if(paramDecl == nullptr)
            {
                paramDecl = new DeclStmt(new Id(identifiers->lookup(((IdentifierSymbolEntry*)curr->getId()->getSymPtr())->getName())), curr->getId());
                paramDecl->setIsParam();
            }
            else
            {
                DeclStmt* newDecl = new DeclStmt(new Id(identifiers->lookup(((IdentifierSymbolEntry*)curr->getId()->getSymPtr())->getName())), curr->getId());
                newDecl->setIsParam();
                paramDecl->setNext(newDecl);
            }
            paramsType.push_back(curr->getId()->getSymPtr()->getType());
            curr = (DeclStmt*)(curr->getNext());
        }
        SymbolEntry *se;
        se = identifiers->lookup($2);
        assert(se != nullptr);
        FunctionType* tmp = (FunctionType*)(se->getType());
        tmp->setParamsType(paramsType);
        if(paramDecl)
            ((SeqNode*)$8)->addStmtFront(paramDecl);
        $$ = new FunctionDef(se,new CompoundStmt($8), (DeclStmt*)$5);
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();
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
    ;
FuncDefParams
    : %empty { $$ = nullptr; }
    | FuncDefParam {
        $$ = $1;
    }
    | FuncDefParams COMMA FuncDefParam {
        $$ = $1;
        $$->setNext($3);
    }
    ;
FuncDefParam
    : Type ID {
        SymbolEntry* se;
        se = new IdentifierSymbolEntry($1, $2, identifiers->getLevel());
        identifiers->install($2, se);
        $$ = new DeclStmt(new Id(se));
        delete []$2;
    }
    | Type ID ASSIGN Exp {
        SymbolEntry* se;
        se = new IdentifierSymbolEntry($1, $2, identifiers->getLevel());
        identifiers->install($2, se);
        $$ = new DeclStmt(new Id(se), $4);
        delete []$2;
    }
    // | Type ID FuncArr {

    // }
    ;
// FuncArr
//     : LBRACKET RBRACKET {

//     }
//     | FuncArr LBRACKET Exp RBRACKET {

//     }
//     ;
FuncCallExp
    : ID LPAREN ExpList RPAREN {
        SymbolEntry *se;
        se = identifiers->lookup($1);
        assert(se != nullptr);
        SymbolEntry* res = new TemporarySymbolEntry(((FunctionType*)se->getType())->getRetType(), SymbolTable::getLabel());
        $$ = new FuncCallExp(se, res, $3);
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

// TODO: array
// ArrConstDef
//     : ArrConstDef LBRACKET ConstExp RBRACKET {

//     }
//     | LBRACKET ConstExp RBRACKET {

//     }
//     ;
// ArrVarDef
//     : ArrVarDef LBRACKET Exp RBRACKET {

//     }
//     | LBRACKET Exp RBRACKET {

//     }
//     ;
// ArrDef
//     : ArrDef LBRACKET Exp RBRACKET {

//     }
//     | LBRACKET Exp RBRACKET {

//     }
%%

int yyerror(char const* message)
{
    std::cerr<<message<<std::endl;
    return -1;
}
