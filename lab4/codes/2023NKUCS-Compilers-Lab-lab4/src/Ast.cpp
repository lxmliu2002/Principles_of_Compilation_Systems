#include "Ast.h"
#include "SymbolTable.h"
#include <string>
#include "Type.h"

using namespace std;


extern FILE *yyout;// 声明一个外部 FILE* 指针 yyout
int Node::counter = 0;// 初始化 Node 类的静态成员变量 counter 为 0

Node::Node()// Node 类的构造函数
{
    seq = counter++;// 为每个 Node 对象分配一个唯一的序号
    next = nullptr;
}

void Node::setNext(Node *node)// 设置 Node 的下一个指针
{
    Node *temp = this;
    while (temp->getNext())
    {
        temp = temp->getNext();
    }
    temp->next = node;
}

Node *Node::getNext()// 获取 Node 的下一个指针
{
    return next;
}

void Ast::output()// Ast 类的输出函数
{
    fprintf(yyout, "program\n");
    if(root != nullptr)
        root->output(4);// 调用根节点的输出方法，缩进级别为 4
}

void BinaryExpr::output(int level)// BinaryExpr 类的输出函数
{
    string op_str;
    switch (op)
    {
    case ADD:
        op_str = "add";
        break;
    case SUB:
        op_str = "sub";
        break;
    case MUL:
        op_str = "mul";
        break;
    case DIV:
        op_str = "div";
        break;
    case MOD:
        op_str = "mod";
        break;
    case AND:
        op_str = "and";
        break;
    case OR:
        op_str = "or";
        break;
    case LESS:
        op_str = "less";
        break;
    case LESSEQUAL:
        op_str = "lessequal";
        break;
    case GREATER:
        op_str = "greater";
        break;
    case GREATEREQUAL:
        op_str = "greaterequal";
        break;
    case EQUAL:
        op_str = "equal";
        break;
    case NOTEQUAL:
        op_str = "notequal";
        break;
    }
    fprintf(yyout, "%*cBinaryExpr\top: %s\n", level, ' ', op_str.c_str());
    expr1->output(level + 4);// 递归输出左子表达式
    expr2->output(level + 4);// 递归输出右子表达式
}

int BinaryExpr::getValue()// 获取 BinaryExpr 的值
{
    int value = 0;
    switch (op)
    {
    case ADD:
        value = expr1->getValue() + expr2->getValue();
        break;
    case SUB:
        value = expr1->getValue() - expr2->getValue();
        break;
    case MUL:
        value = expr1->getValue() * expr2->getValue();
        break;
    case DIV:
        value = expr1->getValue() / expr2->getValue();
        break;
    case MOD:
        value = expr1->getValue() % expr2->getValue();
        break;
    case AND:
        value = expr1->getValue() && expr2->getValue();
        break;
    case OR:
        value = expr1->getValue() || expr2->getValue();
        break;
    case LESS:
        value = expr1->getValue() < expr2->getValue();
        break;
    case LESSEQUAL:
        value = expr1->getValue() <= expr2->getValue();
        break;
    case GREATER:
        value = expr1->getValue() > expr2->getValue();
        break;
    case GREATEREQUAL:
        value = expr1->getValue() >= expr2->getValue();
        break;
    case EQUAL:
        value = expr1->getValue() == expr2->getValue();
        break;
    case NOTEQUAL:
        value = expr1->getValue() != expr2->getValue();
        break;
    }
    return value;
}

void Constant::output(int level)// Constant 类的输出函数
{
    string type, value;
    type = symbolEntry->getType()->toStr();
    value = symbolEntry->toStr();
    fprintf(yyout, "%*cIntegerLiteral\tvalue: %s\ttype: %s\n", level, ' ',
            value.c_str(), type.c_str());
}

void Id::output(int level)// Id 类的输出函数
{
    string name, type;
    int scope;
    name = symbolEntry->toStr();
    type = symbolEntry->getType()->toStr();
    scope = dynamic_cast<IdentifierSymbolEntry*>(symbolEntry)->getScope();
    fprintf(yyout, "%*cId\tname: %s\tscope: %d\ttype: %s\n", level, ' ',
            name.c_str(), scope, type.c_str());
}

void FuncCallExp::output(int level)// FuncCallExp 类的输出函数
{
    string name, type;
    int scope;
    name = symbolEntry->toStr();            //获取函数名
    type = symbolEntry->getType()->toStr(); //获取函数返回值类型
    scope = dynamic_cast<IdentifierSymbolEntry *>(symbolEntry)->getScope();
    fprintf(yyout, "%*cFuncCallExpr\tfunction name: %s\tscope: %d\ttype: %s\n",
            level, ' ', name.c_str(), scope, type.c_str());
    Node *temp = param;
    while (temp)
    {
        temp->output(level + 4);// 递归输出参数节点
        temp = temp->getNext();
    }
}

void ExprStmt::output(int level)// ExprStmt 类的输出函数
{
    fprintf(yyout, "%*cExprStmt\n", level, ' ');
    expr->output(level + 4);// 递归输出表达式
}

void UnaryExpr::output(int level)// UnaryExpr 类的输出函数
{
    string op_str;
    switch (op)
    {
    case ADD:
        op_str = "add";
        break;
    case SUB:
        op_str = "minus";
        break;
    case NOT:
        op_str = "not";
        break;
    }
    fprintf(yyout, "%*cUnaryExpr\top: %s\n", level, ' ', op_str.c_str());
    expr->output(level + 4);// 递归输出子表达式
}

void CompoundStmt::output(int level)// CompoundStmt 类的输出函数
{
    fprintf(yyout, "%*cCompoundStmt\n", level, ' ');
    if (stmt)
    {
        stmt->output(level + 4);// 递归输出语句节点
    }
}

void SeqNode::output(int level)// SeqNode 类的输出函数
{
    //fprintf(yyout, "%*cSequence\n", level, ' ');
    stmt1->output(level);// 输出第一个语句节点
    stmt2->output(level);// 输出第二个语句节点
}

void DeclStmt::output(int level)// DeclStmt 类的输出函数
{
    fprintf(yyout, "%*cDeclStmt\n", level, ' ');
    id->output(level + 4);// 输出标识符节点
    if (expr)// 如果有表达式节点，递归输出
    {
        expr->output(level + 4);
    }
    if (this->getNext())
    {
        this->getNext()->output(level);// 递归输出下一个语句节点
    }
}

void EmptyStmt::output(int level)// EmptyStmt 类的输出函数
{
    fprintf(yyout, "%*cEmptyStmt\n", level, ' ');
}

void IfStmt::output(int level)// IfStmt 类的输出函数
{
    fprintf(yyout, "%*cIfStmt\n", level, ' ');
    cond->output(level + 4);// 输出条件表达式节点
    thenStmt->output(level + 4);// 输出条件为真时的语句节点
}

void IfElseStmt::output(int level)// IfElseStmt 类的输出函数
{
    fprintf(yyout, "%*cIfElseStmt\n", level, ' ');
    cond->output(level + 4);// 输出条件表达式节点
    thenStmt->output(level + 4);// 输出条件为真时的语句节点
    elseStmt->output(level + 4);// 输出条件为假时的语句节点
}

void WhileStmt::output(int level)// WhileStmt 类的输出函数
{
    fprintf(yyout, "%*cWhileStmt\n", level, ' ');
    cond->output(level + 4);// 输出循环条件表达式节点
    stmt->output(level + 4);// 输出循环体语句节点
}

void BreakStmt::output(int level)// BreakStmt 类的输出函数
{
    fprintf(yyout, "%*cBreakStmt\n", level, ' ');
}

void ContinueStmt::output(int level)// ContinueStmt 类的输出函数
{
    fprintf(yyout, "%*cContinueStmt\n", level, ' ');
}

void ReturnStmt::output(int level)// ReturnStmt 类的输出函数
{
    fprintf(yyout, "%*cReturnStmt\n", level, ' ');
    retValue->output(level + 4);// 递归输出返回值表达式节点
}

void AssignStmt::output(int level)// AssignStmt 类的输出函数
{
    fprintf(yyout, "%*cAssignStmt\n", level, ' ');
    lval->output(level + 4);// 输出左值表达式节点
    expr->output(level + 4);// 输出右值表达式节点
}

void FunctionDef::output(int level)// FunctionDef 类的输出函数
{
    string name, type;
    name = se->toStr();
    type = se->getType()->toStr();
    fprintf(yyout, "%*cFunctionDefine function name: %s type: %s\n", level, ' ',
            name.c_str(), type.c_str());
    if (FuncDefParams)
    {
        FuncDefParams->output(level + 4);// 递归输出参数节点
    }
    stmt->output(level + 4);// 输出函数体语句节点
}
