#ifndef __AST_H__
#define __AST_H__

#include <fstream>

class SymbolEntry;

// 结点类
class Node
{
private:
    static int counter;
    int seq;

public:
    // 结点又指向下一个结点，用以表示表达式声明列表，函数参数列表等若干个表达式拼接而成的表达式
    Node *next;
    Node();
    int getSeq() const { return seq; };
    virtual void output(int level) = 0; // 等待子类重载实现
    // 添加 next 结点
    void setNext(Node *node);
    // 获取当前结点的 next 结点
    Node *getNext();
};

// 细分大类一：表达式结点类
class ExprNode : public Node
{
protected:
    SymbolEntry *symbolEntry; // 与符号表中的符号条目相关联

public:
    ExprNode(SymbolEntry *symbolEntry) : symbolEntry(symbolEntry){};
    virtual int getValue() { return 0; };
    SymbolEntry *getSymbolEntry() { return symbolEntry; }
};

// 二进制表达式节点，表示诸如加法、减法等操作
class BinaryExpr : public ExprNode
{
private:
    int op;          // 操作类型（加法、减法等）
    ExprNode *expr1; // 左侧操作数
    ExprNode *expr2; // 右侧操作数

public:
    enum
    {
        ADD,
        SUB,
        MUL,
        DIV,
        MOD,
        AND,
        OR,
        LESS,
        LESSEQUAL,
        GREATER,
        GREATEREQUAL,
        EQUAL,
        NOTEQUAL
    };
    BinaryExpr(SymbolEntry *se, int op, ExprNode *expr1, ExprNode *expr2)
        : ExprNode(se), op(op), expr1(expr1), expr2(expr2){};
    void output(int level);
    int getValue();
};

// 一元表达式节点，表示诸如一元减法、一元加法、逻辑非等操作
class UnaryExpr : public ExprNode
{
private:
    int op;          // 操作类型（一元加法、一元减法、逻辑非等）
    ExprNode *expr;  // 操作数

public:
    enum
    {
        ADD,
        SUB,
        NOT
    };
    UnaryExpr(SymbolEntry *se, int op, ExprNode *expr) : ExprNode(se), op(op), expr(expr){};
    void output(int level);
};

// 函数调用表达式节点，表示函数调用操作
class FuncCallExp : public ExprNode
{
private:
    ExprNode *param; // 函数参数

public:
    FuncCallExp(SymbolEntry *se, ExprNode *param = nullptr)
        : ExprNode(se), param(param){};
    void output(int level);
};

// 常数表达式节点，表示常数值
class Constant : public ExprNode
{
public:
    Constant(SymbolEntry *se) : ExprNode(se){};
    void output(int level);
};

// 标识符表达式节点，表示变量或标识符
class Id : public ExprNode
{
public:
    Id(SymbolEntry *se) : ExprNode(se){};
    void output(int level);
};

// 细分大类二：语句结点类
class StmtNode : public Node
{

};

// 复合语句节点，表示由多个语句组成的语句块
class CompoundStmt : public StmtNode
{
private:
    StmtNode *stmt;

public:
    CompoundStmt(StmtNode *stmt) : stmt(stmt){};
    void output(int level);
};

// 顺序语句节点，表示两个语句按顺序执行
class SeqNode : public StmtNode
{
private:
    StmtNode *stmt1;
    StmtNode *stmt2;

public:
    SeqNode(StmtNode *stmt1, StmtNode *stmt2) : stmt1(stmt1), stmt2(stmt2){};
    void output(int level);
};

// 表达式语句节点，表示单个表达式作为语句
class ExprStmt : public StmtNode
{
private:
    ExprNode *expr;

public:
    ExprStmt(ExprNode *expr) : expr(expr){};
    void output(int level);
};

// 声明语句节点，表示变量或标识符的声明语句
class DeclStmt : public StmtNode
{
private:
    Id *id;        // 声明的变量或标识符
    ExprNode *expr; // 赋值给标识符的表达式，如果为空，代表只声明，不赋初值

public:
    DeclStmt(Id *id, ExprNode *expr = nullptr) : id(id), expr(expr){};
    void output(int level);
    Id *getId() { return id; }
};

// If语句节点，表示条件语句
class IfStmt : public StmtNode
{
private:
    ExprNode *cond;     // 条件表达式
    StmtNode *thenStmt; // 条件满足时执行的语句

public:
    IfStmt(ExprNode *cond, StmtNode *thenStmt) : cond(cond), thenStmt(thenStmt){};
    void output(int level);
};

// If-Else语句节点，表示条件语句带有一个分支
class IfElseStmt : public StmtNode
{
private:
    ExprNode *cond;      // 条件表达式
    StmtNode *thenStmt;  // 条件满足时执行的语句
    StmtNode *elseStmt;  // 条件不满足时执行的语句

public:
    IfElseStmt(ExprNode *cond, StmtNode *thenStmt, StmtNode *elseStmt) : cond(cond), thenStmt(thenStmt), elseStmt(elseStmt) {};
    void output(int level);
};

// While循环语句节点
class WhileStmt : public StmtNode
{
private:
    ExprNode *cond; // 循环条件
    StmtNode *stmt; // 循环体语句

public:
    WhileStmt(ExprNode *cond, StmtNode *stmt) : cond(cond), stmt(stmt){};
    void output(int level);
};

// Break语句节点，表示中断当前循环
class BreakStmt : public StmtNode
{
public:
    BreakStmt(){};
    void output(int level);
};

// Continue语句节点，表示跳过当前循环的剩余部分，继续下一次循环
class ContinueStmt : public StmtNode
{
public:
    ContinueStmt(){};
    void output(int level);
};

// Return语句节点，表示函数返回语句
class ReturnStmt : public StmtNode
{
private:
    ExprNode *retValue; // 返回值表达式

public:
    ReturnStmt(ExprNode *retValue) : retValue(retValue) {};
    void output(int level);
};

// 赋值语句节点，表示变量或标识符的赋值语句
class AssignStmt : public StmtNode
{
private:
    ExprNode *lval; // 被赋值的变量或标识符
    ExprNode *expr; // 赋给变量或标识符的表达式

public:
    AssignStmt(ExprNode *lval, ExprNode *expr) : lval(lval), expr(expr) {};
    void output(int level);
};

// 函数定义节点，表示函数的定义
class FunctionDef : public StmtNode
{
private:
    SymbolEntry *se;      // 函数符号表条目
    StmtNode *stmt;       // 函数体语句
    DeclStmt *FuncDefParams; // 函数参数声明

public:
    FunctionDef(SymbolEntry *se, StmtNode *stmt, DeclStmt *FuncDefParams = nullptr) : se(se), stmt(stmt), FuncDefParams(FuncDefParams){};
    void output(int level);
};

// 空语句节点，表示一个空语句
class EmptyStmt : public StmtNode
{
public:
    EmptyStmt(){};
    void output(int level);
};

// 树
class Ast
{
private:
    Node *root; // AST 的根节点

public:
    Ast() { root = nullptr; }
    void setRoot(Node *n) { root = n; }
    void output(); // 输出整棵树
};

#endif
