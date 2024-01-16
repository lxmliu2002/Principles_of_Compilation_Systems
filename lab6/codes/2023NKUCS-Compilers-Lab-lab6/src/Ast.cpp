#include "Ast.h"
#include <stack>
#include <string>
#include "IRBuilder.h"
#include "Instruction.h"
#include "SymbolTable.h"
#include "Type.h"
#include "Unit.h"
extern Unit unit;
extern MachineUnit mUnit;

#include <iostream>

extern FILE *yyout;
int Node::counter = 0;
IRBuilder *Node::builder;

Node::Node()
{
    seq = counter++;
    next = nullptr;
}

void Node::setNext(Node *node)
{
    Node *n = this;
    while (n->getNext())
    {
        n = n->getNext();
    }
    if (n == this)
    {
        this->next = node;
    }
    else
    {
        n->setNext(node);
    }
}

void Node::backPatch(std::vector<Instruction *> &list, BasicBlock *bb)
{
    for (auto &inst : list)
    {
        if (inst->isCond())
        {
            dynamic_cast<CondBrInstruction *>(inst)->setTrueBranch(bb);
        }
        else if (inst->isUncond())
        {
            dynamic_cast<UncondBrInstruction *>(inst)->setBranch(bb);
        }
    }
}

std::vector<Instruction *> Node::merge(std::vector<Instruction *> &list1, std::vector<Instruction *> &list2)
{
    std::vector<Instruction *> res(list1);
    res.insert(res.end(), list2.begin(), list2.end());
    return res;
}

void Ast::genCode(Unit *unit)
{
    IRBuilder *builder = new IRBuilder(unit);
    Node::setIRBuilder(builder);
    root->genCode();
}

void FunctionDef::genCode()
{
    Unit *unit = builder->getUnit();
    Function *func = new Function(unit, se);
    BasicBlock *entry = func->getEntry();

    builder->setInsertBB(entry);
    if (decl)
    {
        decl->genCode();
    }
    if (stmt)
    {
        stmt->genCode();
    }

    for (auto block = func->begin(); block != func->end(); block++)
    {
        Instruction *i = (*block)->begin();
        Instruction *last = (*block)->rbegin();
        while (i != last)
        {
            if (i->isCond() || i->isUncond())
            {
                (*block)->remove(i);
            }
            i = i->getNext();
        }
        if (last->isCond())
        {
            BasicBlock *truebranch, *falsebranch;
            truebranch = dynamic_cast<CondBrInstruction *>(last)->getTrueBranch();
            falsebranch = dynamic_cast<CondBrInstruction *>(last)->getFalseBranch();
            if (truebranch->empty())
            {
                new RetInstruction(nullptr, truebranch);
            }
            else if (falsebranch->empty())
            {
                new RetInstruction(nullptr, falsebranch);
            }
            (*block)->addSucc(truebranch);
            (*block)->addSucc(falsebranch);
            truebranch->addPred(*block);
            falsebranch->addPred(*block);
        }
        else if (last->isUncond())
        {
            BasicBlock *dst = dynamic_cast<UncondBrInstruction *>(last)->getBranch();
            (*block)->addSucc(dst);
            dst->addPred(*block);
            if (dst->empty())
            {
                if (((FunctionType *)(se->getType()))->getRetType() == TypeSystem::intType)
                {
                    new RetInstruction(new Operand(new ConstantSymbolEntry(TypeSystem::intType, 0)), dst);
                }
                else if (((FunctionType *)(se->getType()))->getRetType() == TypeSystem::floatType)
                {
                    new RetInstruction(new Operand(new ConstantSymbolEntry(TypeSystem::floatType, 0)), dst);
                }
                else if (((FunctionType *)(se->getType()))->getRetType() == TypeSystem::voidType)
                {
                    new RetInstruction(nullptr, dst);
                }
            }
        }
        else if (!last->isRet())
        {
            if (((FunctionType *)(se->getType()))->getRetType() == TypeSystem::voidType)
            {
                new RetInstruction(nullptr, *block);
            }
        }
    }
}

BinaryExpr::BinaryExpr(SymbolEntry *se, int op, ExprNode *expr1, ExprNode *expr2) : ExprNode(se, BINARYEXPR), op(op), expr1(expr1), expr2(expr2)
{
    dst = new Operand(se);
    std::string op_str;
    switch (op)
    {
    case ADD:
        op_str = "+";
        break;
    case SUB:
        op_str = "-";
        break;
    case MUL:
        op_str = "*";
        break;
    case DIV:
        op_str = "/";
        break;
    case MOD:
        op_str = "%";
        break;
    case AND:
        op_str = "&&";
        break;
    case OR:
        op_str = "||";
        break;
    case LESS:
        op_str = "<";
        break;
    case LESSEQUAL:
        op_str = "<=";
        break;
    case GREATER:
        op_str = ">";
        break;
    case GREATEREQUAL:
        op_str = ">=";
        break;
    case EQUAL:
        op_str = "==";
        break;
    case NOTEQUAL:
        op_str = "!=";
        break;
    }
    if (expr1->getType()->isVoid() || expr2->getType()->isVoid())
    {
        fprintf(stderr, "invalid operand of type \'void\' to binary \'opeartor%s\'\n", op_str.c_str());
    }
    if (op >= BinaryExpr::AND && op <= BinaryExpr::NOTEQUAL)
    {
        type = TypeSystem::boolType;
        if (op == BinaryExpr::AND || op == BinaryExpr::OR)
        {
            if (expr1->getType()->isInt() && expr1->getType()->getSize() == 32)
            {
                ImplicitCastExpr *temp = new ImplicitCastExpr(expr1);
                this->expr1 = temp;
            }
            else if (expr1->getType()->isFloat())
            {
                SymbolEntry *zero = new ConstantSymbolEntry(TypeSystem::floatType, 0);
                SymbolEntry *temp = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
                BinaryExpr *cmpZero = new BinaryExpr(temp, BinaryExpr::NOTEQUAL, expr1, new Constant(zero));
                this->expr1 = cmpZero;
            }

            if (expr2->getType()->isInt() && expr2->getType()->getSize() == 32)
            {
                ImplicitCastExpr *temp = new ImplicitCastExpr(expr2);
                this->expr2 = temp;
            }
            else if (expr2->getType()->isFloat())
            {
                SymbolEntry *zero = new ConstantSymbolEntry(TypeSystem::floatType, 0);
                SymbolEntry *temp = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
                BinaryExpr *cmpZero = new BinaryExpr(temp, BinaryExpr::NOTEQUAL, expr2, new Constant(zero));
                this->expr2 = cmpZero;
            }
        }
        // TODO optimize literal number
        if (op == BinaryExpr::LESS || op == BinaryExpr::LESSEQUAL || op == BinaryExpr::GREATER || op == BinaryExpr::GREATEREQUAL || op == BinaryExpr::EQUAL || op == BinaryExpr::NOTEQUAL)
        {
            if (expr1->getType()->isFloat() && expr2->getType()->isInt())
            {
                ImplicitCastExpr *temp = new ImplicitCastExpr(expr2, TypeSystem::floatType);
                this->expr2 = temp;
            }
            else if (expr1->getType()->isInt() && expr2->getType()->isFloat())
            {
                ImplicitCastExpr *temp = new ImplicitCastExpr(expr1, TypeSystem::floatType);
                this->expr1 = temp;
                type = TypeSystem::floatType;
            }
        }
    }
    else if (expr1->getType()->isFloat() && expr2->getType()->isInt())
    {
        if (op == BinaryExpr::MOD)
        {
            fprintf(stderr, "Operands of `mod` must be both integers");
        }
        ImplicitCastExpr *temp = new ImplicitCastExpr(expr2, TypeSystem::floatType);
        this->expr2 = temp;
        type = TypeSystem::floatType;
    }
    else if (expr1->getType()->isInt() && expr2->getType()->isFloat())
    {
        if (op == BinaryExpr::MOD)
        {
            fprintf(stderr, "Operands of `mod` must be both integers");
        }
        ImplicitCastExpr *temp = new ImplicitCastExpr(expr1, TypeSystem::floatType);
        this->expr1 = temp;
        type = TypeSystem::floatType;
    }
    else if (expr1->getType()->isFloat() && expr2->getType()->isFloat())
    {
        if (op == BinaryExpr::MOD)
        {
            fprintf(stderr, "Operands of `mod` must be both integers");
        }
        type = TypeSystem::floatType;
    }
    else
    {
        type = TypeSystem::intType;
    }
};

void BinaryExpr::genCode()
{
    BasicBlock *bb = builder->getInsertBB();
    Function *func = bb->getParent();
    if (op == AND)
    {
        BasicBlock *trueBB = new BasicBlock(func);
        expr1->genCode();
        backPatch(expr1->trueList(), trueBB);
        builder->setInsertBB(trueBB);
        expr2->genCode();
        true_list = expr2->trueList();
        false_list = merge(expr1->falseList(), expr2->falseList());
    }
    else if (op == OR)
    {
        BasicBlock *trueBB = new BasicBlock(func);
        expr1->genCode();
        backPatch(expr1->falseList(), trueBB);
        builder->setInsertBB(trueBB);
        expr2->genCode();
        true_list = merge(expr1->trueList(), expr2->trueList());
        false_list = expr2->falseList();
    }
    else if (op >= LESS && op <= NOTEQUAL)
    {
        expr1->genCode();
        expr2->genCode();
        Operand *src1 = expr1->getOperand();
        Operand *src2 = expr2->getOperand();
        if (src1->getType()->getSize() == 1)
        {
            Operand *dst = new Operand(new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel()));
            new ZextInstruction(dst, src1, bb);
            src1 = dst;
        }
        if (src2->getType()->getSize() == 1)
        {
            Operand *dst = new Operand(new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel()));
            new ZextInstruction(dst, src2, bb);
            src2 = dst;
        }
        int cmpopcode = -1;
        switch (op)
        {
        case LESS:
            cmpopcode = CmpInstruction::L;
            break;
        case LESSEQUAL:
            cmpopcode = CmpInstruction::LE;
            break;
        case GREATER:
            cmpopcode = CmpInstruction::G;
            break;
        case GREATEREQUAL:
            cmpopcode = CmpInstruction::GE;
            break;
        case EQUAL:
            cmpopcode = CmpInstruction::E;
            break;
        case NOTEQUAL:
            cmpopcode = CmpInstruction::NE;
            break;
        }
        new CmpInstruction(cmpopcode, dst, src1, src2, bb);
        BasicBlock *truebb, *falsebb, *tempbb;
        truebb = new BasicBlock(func);
        falsebb = new BasicBlock(func);
        tempbb = new BasicBlock(func);
        true_list.push_back(new CondBrInstruction(truebb, tempbb, dst, bb));
        false_list.push_back(new UncondBrInstruction(falsebb, tempbb));
    }
    else if (op >= ADD && op <= MOD)
    {
        expr1->genCode();
        expr2->genCode();
        Operand *src1 = expr1->getOperand();
        Operand *src2 = expr2->getOperand();
        int opcode = -1;
        switch (op)
        {
        case ADD:
            opcode = BinaryInstruction::ADD;
            break;
        case SUB:
            opcode = BinaryInstruction::SUB;
            break;
        case MUL:
            opcode = BinaryInstruction::MUL;
            break;
        case DIV:
            opcode = BinaryInstruction::DIV;
            break;
        case MOD:
            opcode = BinaryInstruction::MOD;
            break;
        }
        new BinaryInstruction(opcode, dst, src1, src2, bb);
    }
}

ExprNode *BinaryExpr::getLeft()
{
    return this->expr1;
}

ExprNode *BinaryExpr::getRight()
{
    return this->expr2;
}

ExprNode *UnaryExpr::getSubExpr()
{
    return this->expr;
}
void Constant::genCode()
{
    // we don't need to generate code.
}

void Id::genCode()
{
    BasicBlock *bb = builder->getInsertBB();
    Operand *addr = dynamic_cast<IdentifierSymbolEntry *>(symbolEntry)->getAddr();
    if (type->isInt() || type->isFloat())
    {
        new LoadInstruction(dst, addr, bb);
    }
    else if (type->isArray())
    {
        if (arrIdx)
        {
            Type *type = ((ArrayType *)(this->type))->getElementType();
            Type *type1 = this->type;
            Operand *tempSrc = addr;
            Operand *tempDst = dst;
            ExprNode *idx = arrIdx;
            bool flag = false;
            bool pointer = false;
            bool firstFlag = true;
            while (true)
            {
                if (((ArrayType *)type1)->getLength() == -1)
                {
                    Operand *dst1 = new Operand(new TemporarySymbolEntry(new PointerType(type), SymbolTable::getLabel()));
                    tempSrc = dst1;
                    new LoadInstruction(dst1, addr, bb);
                    flag = true;
                    firstFlag = false;
                }
                if (!idx)
                {
                    Operand *dst1 = new Operand(new TemporarySymbolEntry(new PointerType(type), SymbolTable::getLabel()));
                    Operand *idx = new Operand(new ConstantSymbolEntry(TypeSystem::intType, 0));
                    new GepInstruction(dst1, tempSrc, idx, bb);
                    tempDst = dst1;
                    pointer = true;
                    break;
                }

                idx->genCode();
                auto gep = new GepInstruction(tempDst, tempSrc, idx->getOperand(), bb, flag);
                if (!flag && firstFlag)
                {
                    gep->setFirst();
                    firstFlag = false;
                }
                if (flag)
                {
                    flag = false;
                }
                if (type == TypeSystem::intType || type == TypeSystem::constIntType || type == TypeSystem::floatType || type == TypeSystem::constFloatType)
                {
                    break;
                }
                type = ((ArrayType *)type)->getElementType();
                type1 = ((ArrayType *)type1)->getElementType();
                tempSrc = tempDst;
                tempDst = new Operand(new TemporarySymbolEntry(new PointerType(type), SymbolTable::getLabel()));
                idx = (ExprNode *)(idx->getNext());
            }
            dst = tempDst;
            if (!left && !pointer)
            {
                Operand *dst1 = new Operand(new TemporarySymbolEntry(type, SymbolTable::getLabel()));
                new LoadInstruction(dst1, dst, bb);
                dst = dst1;
            }
        }
        else
        {
            if (((ArrayType *)(this->type))->getLength() == -1)
            {
                Operand *dst1 = new Operand(new TemporarySymbolEntry(new PointerType(((ArrayType *)(this->type))->getElementType()), SymbolTable::getLabel()));
                new LoadInstruction(dst1, addr, bb);
                dst = dst1;
            }
            else
            {
                Operand *idx = new Operand(new ConstantSymbolEntry(TypeSystem::intType, 0));
                auto gep = new GepInstruction(dst, addr, idx, bb);
                gep->setFirst();
            }
        }
    }
}

void IfStmt::genCode()
{
    Function *func;
    BasicBlock *then_bb, *end_bb;

    func = builder->getInsertBB()->getParent();
    then_bb = new BasicBlock(func);
    end_bb = new BasicBlock(func);

    cond->genCode();

    backPatch(cond->trueList(), then_bb);
    backPatch(cond->falseList(), end_bb);

    builder->setInsertBB(then_bb);
    thenStmt->genCode();
    then_bb = builder->getInsertBB();
    new UncondBrInstruction(end_bb, then_bb);

    builder->setInsertBB(end_bb);
}

void IfElseStmt::genCode()
{
    Function *func;
    BasicBlock *then_bb, *else_bb, *end_bb;
    func = builder->getInsertBB()->getParent();
    then_bb = new BasicBlock(func);
    else_bb = new BasicBlock(func);
    end_bb = new BasicBlock(func);

    cond->genCode();
    backPatch(cond->trueList(), then_bb);
    backPatch(cond->falseList(), else_bb);

    builder->setInsertBB(then_bb);
    thenStmt->genCode();
    then_bb = builder->getInsertBB();
    new UncondBrInstruction(end_bb, then_bb);

    builder->setInsertBB(else_bb);
    elseStmt->genCode();
    else_bb = builder->getInsertBB();
    new UncondBrInstruction(end_bb, else_bb);

    builder->setInsertBB(end_bb);
}

void CompoundStmt::genCode()
{
    if (stmt)
    {
        stmt->genCode();
    }
}

void SeqNode::genCode()
{
    stmt1->genCode();
    stmt2->genCode();
}

void DeclStmt::genCode()
{
    IdentifierSymbolEntry *se = dynamic_cast<IdentifierSymbolEntry *>(id->getSymbolEntry());
    if (se->isGlobal())
    {
        Operand *addr;
        SymbolEntry *addr_se;
        addr_se = new IdentifierSymbolEntry(*se);
        addr_se->setType(new PointerType(se->getType()));
        addr = new Operand(addr_se);
        se->setAddr(addr);
        unit.insertGlobal(se);
        mUnit.insertGlobal(se);
    }
    else if (se->isLocal() || se->isParam())
    {
        Function *func = builder->getInsertBB()->getParent();
        BasicBlock *entry = func->getEntry();
        Instruction *alloca;
        Operand *addr;
        SymbolEntry *addr_se;
        Type *type;
        type = new PointerType(se->getType());
        addr_se = new TemporarySymbolEntry(type, SymbolTable::getLabel());
        addr = new Operand(addr_se);
        alloca = new AllocaInstruction(addr, se);
        entry->insertFront(alloca);
        Operand *temp = nullptr;
        if (se->isParam())
        {
            temp = se->getAddr();
        }
        se->setAddr(addr);
        if (expr)
        {
            if (expr->isInitValueListExpr())
            {
                Operand *init = nullptr;
                BasicBlock *bb = builder->getInsertBB();
                ExprNode *temp = expr;
                std::stack<ExprNode *> stk;
                std::vector<int> idx;
                idx.push_back(0);
                while (temp)
                {
                    if (temp->isInitValueListExpr())
                    {
                        stk.push(temp);
                        idx.push_back(0);
                        temp = ((InitValueListExpr *)temp)->getExpr();
                        continue;
                    }
                    else
                    {
                        temp->genCode();
                        Type *type = ((ArrayType *)(se->getType()))->getElementType();
                        Operand *tempSrc = addr;
                        Operand *tempDst;
                        Operand *index;
                        bool flag = true;
                        int i = 1;
                        while (true)
                        {
                            tempDst = new Operand(new TemporarySymbolEntry(new PointerType(type), SymbolTable::getLabel()));
                            index = (new Constant(new ConstantSymbolEntry(TypeSystem::intType, idx[i++])))->getOperand();
                            auto gep = new GepInstruction(tempDst, tempSrc, index, bb);
                            gep->setInit(init);
                            if (flag)
                            {
                                gep->setFirst();
                                flag = false;
                            }
                            if (type == TypeSystem::intType || type == TypeSystem::constIntType || type == TypeSystem::floatType || type == TypeSystem::constFloatType)
                            {
                                gep->setLast();
                                init = tempDst;
                                break;
                            }
                            type = ((ArrayType *)type)->getElementType();
                            tempSrc = tempDst;
                        }
                        new StoreInstruction(tempDst, temp->getOperand(), bb);
                    }
                    while (true)
                    {
                        if (temp->getNext())
                        {
                            temp = (ExprNode *)(temp->getNext());
                            idx[idx.size() - 1]++;
                            break;
                        }
                        else
                        {
                            temp = stk.top();
                            stk.pop();
                            idx.pop_back();
                            if (stk.empty())
                            {
                                break;
                            }
                        }
                    }
                    if (stk.empty())
                    {
                        break;
                    }
                }
            }
            else
            {
                BasicBlock *bb = builder->getInsertBB();
                expr->genCode();
                Operand *src = expr->getOperand();
                new StoreInstruction(addr, src, bb);
            }
        }
        if (se->isParam())
        {
            BasicBlock *bb = builder->getInsertBB();
            new StoreInstruction(addr, temp, bb);
        }
    }
    if (this->getNext())
    {
        this->getNext()->genCode();
    }
}

void ReturnStmt::genCode()
{
    BasicBlock *bb = builder->getInsertBB();
    Operand *src = nullptr;
    if (retValue)
    {
        retValue->genCode();
        src = retValue->getOperand();
    }
    new RetInstruction(src, bb);
}
void ExprStmt::genCode()
{
    expr->genCode();
}
void ContinueStmt::genCode()
{
    Function *func = builder->getInsertBB()->getParent();
    BasicBlock *bb = builder->getInsertBB();
    new UncondBrInstruction(((WhileStmt *)whileStmt)->get_cond_bb(), bb);
    BasicBlock *continue_next_bb = new BasicBlock(func);
    builder->setInsertBB(continue_next_bb);
}
void BreakStmt::genCode()
{
    Function *func = builder->getInsertBB()->getParent();
    BasicBlock *bb = builder->getInsertBB();
    new UncondBrInstruction(((WhileStmt *)whileStmt)->get_end_bb(), bb);
    BasicBlock *break_next_bb = new BasicBlock(func);
    builder->setInsertBB(break_next_bb);
}
void WhileStmt::genCode()
{
    Function *func;
    BasicBlock *cond_bb, *while_bb, *end_bb, *bb;
    bb = builder->getInsertBB();
    func = builder->getInsertBB()->getParent();
    cond_bb = new BasicBlock(func);
    while_bb = new BasicBlock(func);
    end_bb = new BasicBlock(func);

    this->cond_bb = cond_bb;
    this->end_bb = end_bb;

    new UncondBrInstruction(cond_bb, bb);

    builder->setInsertBB(cond_bb);
    cond->genCode();
    backPatch(cond->trueList(), while_bb);
    backPatch(cond->falseList(), end_bb);

    builder->setInsertBB(while_bb);
    stmt->genCode();
    cond->genCode();
    backPatch(cond->trueList(), while_bb);
    backPatch(cond->falseList(), end_bb);

    builder->setInsertBB(end_bb);
}
void BlankStmt::genCode()
{
}
void InitValueListExpr::genCode()
{
}
void CallExpr::genCode()
{
    std::vector<Operand *> operands;
    ExprNode *temp = param;
    while (temp)
    {
        temp->genCode();
        operands.push_back(temp->getOperand());
        temp = ((ExprNode *)temp->getNext());
    }
    BasicBlock *bb = builder->getInsertBB();
    new CallInstruction(dst, symbolEntry, operands, bb);
}
void UnaryExpr::genCode()
{
    expr->genCode();
    if (op == NOT)
    {
        BasicBlock *bb = builder->getInsertBB();
        Operand *src = expr->getOperand();
        if (expr->getType()->getSize() == 32)
        {
            Operand *temp = new Operand(new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel()));
            new CmpInstruction(CmpInstruction::NE, temp, src, new Operand(new ConstantSymbolEntry(TypeSystem::intType, 0)), bb);
            src = temp;
        }
        new XorInstruction(dst, src, bb);
    }
    else if (op == SUB)
    {
        Operand *src2;
        BasicBlock *bb = builder->getInsertBB();
        Operand *src1 = new Operand(new ConstantSymbolEntry(TypeSystem::intType, 0));
        if (expr->getType()->getSize() == 1)
        {
            src2 = new Operand(new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel()));
            new ZextInstruction(src2, expr->getOperand(), bb);
        }
        else
        {
            src2 = expr->getOperand();
        }
        new BinaryInstruction(BinaryInstruction::SUB, dst, src1, src2, bb);
    }
}
void ExprNode::genCode()
{
}

ExprNode *ExprNode::alge_simple(int depth)
{
    int op;
    ExprNode *res = this;
    if (this->isBinaryExpr())
    {
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
        op = ((BinaryExpr *)this)->getOp();
        ExprNode *lhs = ((BinaryExpr *)this)->getLeft(), *rhs = ((BinaryExpr *)this)->getRight();
        if (depth && lhs->isBinaryExpr())
        {
            lhs = lhs->alge_simple(depth - 1);
        }
        if (depth && rhs->isBinaryExpr())
        {
            rhs = rhs->alge_simple(depth - 1);
        }
        switch (op)
        {
        case ADD:
            if (lhs->getSymbolEntry()->isConstant() && ((ConstantSymbolEntry *)(lhs->getSymbolEntry()))->getValue() == 0)
            {
                res = rhs;
            }
            else if (rhs->getSymbolEntry()->isConstant() && ((ConstantSymbolEntry *)(rhs->getSymbolEntry()))->getValue() == 0)
            {
                res = lhs;
            }
            else
            {
                SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
                res = new BinaryExpr(se, ADD, lhs, rhs);
            }
            break;
        case SUB:
            if (rhs->getSymbolEntry()->isConstant() && ((ConstantSymbolEntry *)(rhs->getSymbolEntry()))->getValue() == 0)
            {
                res = lhs;
            }
            else
            {
                SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
                res = new BinaryExpr(se, SUB, lhs, rhs);
            }
            break;
        case MUL:
            if (lhs->getSymbolEntry()->isConstant())
            {
                if (((ConstantSymbolEntry *)(lhs->getSymbolEntry()))->getValue() == 0)
                {
                    SymbolEntry *se = new ConstantSymbolEntry(TypeSystem::intType, 0);
                    res = new Constant(se);
                }
                else if (((ConstantSymbolEntry *)(lhs->getSymbolEntry()))->getValue() == 1)
                {
                    res = rhs;
                }
            }
            else if (rhs->getSymbolEntry()->isConstant())
            {
                if (((ConstantSymbolEntry *)(rhs->getSymbolEntry()))->getValue() == 0)
                {
                    SymbolEntry *se = new ConstantSymbolEntry(TypeSystem::intType, 0);
                    res = new Constant(se);
                }
                else if (((ConstantSymbolEntry *)(rhs->getSymbolEntry()))->getValue() == 1)
                {
                    res = lhs;
                }
            }
            else
            {
                SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
                res = new BinaryExpr(se, MUL, lhs, rhs);
            }
            break;
        case DIV:
            if (rhs->getSymbolEntry()->isConstant() && ((ConstantSymbolEntry *)(lhs->getSymbolEntry()))->getValue() == 1)
            {
                res = lhs;
            }
            else
            {
                SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
                res = new BinaryExpr(se, DIV, lhs, rhs);
            }
            break;
        }
    }
    return res;
}

ExprNode *ExprNode::const_fold()
{
    ExprNode *res = this;
    res = this->alge_simple(5);
    bool flag = true;
    int fconst = res->fold_const(flag);
    if (flag)
    {
        SymbolEntry *se = new ConstantSymbolEntry(TypeSystem::intType, fconst);
        res = new Constant(se);
    }
    return res;
}

int ExprNode::fold_const(bool &flag)
{
    if (this->isBinaryExpr())
    {
        ExprNode *lhs = ((BinaryExpr *)this)->getLeft(), *rhs = ((BinaryExpr *)this)->getRight();
        lhs->fold_const(flag);
        if (flag)
        {
            rhs->fold_const(flag);
        }
        if (flag)
        {
            return ((BinaryExpr *)this)->getValue();
        }
        else
        {
            return 0;
        }
    }
    else if (this->isUnaryExpr())
    {
        ExprNode *hs = ((UnaryExpr *)this)->getSubExpr();
        hs->fold_const(flag);
        if (flag)
        {
            return ((UnaryExpr *)this)->getValue();
        }
        else
        {
            return 0;
        }
    }
    else if (this->isExpr())
    {
        SymbolEntry *sym = this->getSymbolEntry();
        if (sym->isConstant())
        {
            return ((ConstantSymbolEntry *)sym)->getValue();
        }
    }
    flag = 0;
    return 0;
}

bool ContinueStmt::typeCheck(Type *retType)
{
    return false;
}
bool BreakStmt::typeCheck(Type *retType)
{
    return false;
}
bool WhileStmt::typeCheck(Type *retType)
{
    if (stmt)
    {
        return stmt->typeCheck(retType);
    }
    return false;
}
bool BlankStmt::typeCheck(Type *retType)
{
    return false;
}
bool InitValueListExpr::typeCheck(Type *retType)
{
    return false;
}
bool CallExpr::typeCheck(Type *retType)
{
    return false;
}
bool UnaryExpr::typeCheck(Type *retType)
{
    return false;
}

bool ExprStmt::typeCheck(Type *retType)
{
    return false;
}

void AssignStmt::genCode()
{
    BasicBlock *bb = builder->getInsertBB();
    expr->genCode();
    Operand *addr = nullptr;
    if (lval->getOriginType()->isInt() || lval->getOriginType()->isFloat())
    {
        addr = dynamic_cast<IdentifierSymbolEntry *>(lval->getSymbolEntry())->getAddr();
    }
    else if (lval->getOriginType()->isArray())
    {
        ((Id *)lval)->setLeft();
        lval->genCode();
        addr = lval->getOperand();
    }
    Operand *src = expr->getOperand();
    new StoreInstruction(addr, src, bb);
}

bool Ast::typeCheck(Type *retType)
{
    if (root != nullptr)
    {
        return root->typeCheck();
    }
    return false;
}

bool FunctionDef::typeCheck(Type *retType)
{
    SymbolEntry *se = this->getSymbolEntry();
    Type *ret = ((FunctionType *)(se->getType()))->getRetType();
    StmtNode *stmt = this->stmt;
    if (stmt == nullptr)
    {
        if (ret != TypeSystem::voidType)
        {
            fprintf(stderr, "non-void function does not return a value.\n");
        }
        return false;
    }
    if (!stmt->typeCheck(ret))
    {
        return false;
    }
    return false;
}

bool BinaryExpr::typeCheck(Type *retType)
{
    return false;
}

bool Constant::typeCheck(Type *retType)
{
    return false;
}

bool Id::typeCheck(Type *retType)
{
    return false;
}

bool IfStmt::typeCheck(Type *retType)
{
    if (thenStmt)
    {
        return thenStmt->typeCheck(retType);
    }
    return false;
}

bool IfElseStmt::typeCheck(Type *retType)
{
    bool flag1 = false, flag2 = false;
    if (thenStmt)
    {
        flag1 = thenStmt->typeCheck(retType);
    }
    if (elseStmt)
    {
        flag2 = elseStmt->typeCheck(retType);
    }
    return flag1 || flag2;
}

bool CompoundStmt::typeCheck(Type *retType)
{
    if (stmt)
    {
        return stmt->typeCheck(retType);
    }
    return false;
}

bool SeqNode::typeCheck(Type *retType)
{
    bool flag1 = false, flag2 = false;
    if (stmt1)
    {
        flag1 = stmt1->typeCheck(retType);
    }
    if (stmt2)
    {
        flag2 = stmt2->typeCheck(retType);
    }
    return flag1 || flag2;
}

bool DeclStmt::typeCheck(Type *retType)
{
    return false;
}

bool ReturnStmt::typeCheck(Type *retType)
{
    if (!retType)
    {
        fprintf(stderr, "expected unqualified-id\n");
        return true;
    }
    if (!retValue && !retType->isVoid())
    {
        fprintf(stderr, "return-statement with no value, in function returning \'%s\'\n", retType->toStr().c_str());
        return true;
    }
    if (retValue && retType->isVoid())
    {
        fprintf(stderr, "return-statement with a value, in function returning \'void\'\n");
        return true;
    }
    if (!retValue || !retValue->getSymbolEntry())
    {
        fprintf(stderr, "retValue or its symbol entry error\n");
        return true;
    }
    Type *type = retValue->getType();
    if (type != retType)
    {
        fprintf(stderr, "cannot initialize return object of type \'%s\' with an rvalueof type \'%s\'\n", retType->toStr().c_str(), type->toStr().c_str());
        return true;
    }
    return false;
}

bool AssignStmt::typeCheck(Type *retType)
{
    return false;
}

CallExpr::CallExpr(SymbolEntry *se, ExprNode *param) : ExprNode(se), param(param)
{
    dst = nullptr;
    SymbolEntry *s = se;
    int paramCnt = 0;
    ExprNode *temp = param;
    while (temp)
    {
        paramCnt++;
        temp = (ExprNode *)(temp->getNext());
    }
    while (s)
    {
        Type *type = s->getType();
        std::vector<Type *> params = ((FunctionType *)type)->getParamsType();
        if ((long unsigned int)paramCnt == params.size())
        {
            this->symbolEntry = s;
            break;
        }
        s = s->getNext();
    }
    if (symbolEntry)
    {
        Type *type = symbolEntry->getType();
        this->type = ((FunctionType *)type)->getRetType();
        if (this->type != TypeSystem::voidType)
        {
            SymbolEntry *se = new TemporarySymbolEntry(this->type, SymbolTable::getLabel());
            dst = new Operand(se);
        }
        std::vector<Type *> params = ((FunctionType *)type)->getParamsType();
        ExprNode *temp = param;
        ExprNode *temp1 = nullptr;

        for (auto it = params.begin(); it != params.end(); it++)
        {
            if (temp == nullptr)
            {
                fprintf(stderr, "too few arguments to function %s %s\n", symbolEntry->toStr().c_str(), type->toStr().c_str());
                break;
            }
            else if ((*it)->getKind() != temp->getType()->getKind())
            {
                if (((*it)->isFloat() && temp->getType()->isInt()) || ((*it)->isInt() && temp->getType()->isFloat()))
                {
                    ImplicitCastExpr *implicitCastExpr = new ImplicitCastExpr(temp, (*it));
                    implicitCastExpr->setNext((ExprNode *)(temp->getNext()));

                    if (it != params.begin())
                    {
                        temp1->setAdjNext(implicitCastExpr);
                        temp = implicitCastExpr;
                    }
                    else
                    {
                        temp = implicitCastExpr;
                        this->param = implicitCastExpr;
                    }
                }
                else
                {
                    fprintf(stderr, "parameter's type %s can't convert to %s\n", temp->getType()->toStr().c_str(), (*it)->toStr().c_str());
                }
            }
            temp1 = temp;
            temp = (ExprNode *)(temp->getNext());
        }
        if (temp != nullptr)
        {
            fprintf(stderr, "too many arguments to function %s %s\n", symbolEntry->toStr().c_str(), type->toStr().c_str());
        }
    }
    if (((IdentifierSymbolEntry *)se)->isSysy())
    {
        unit.insertDeclare(se);
    }
}

AssignStmt::AssignStmt(ExprNode *lval, ExprNode *expr) : lval(lval), expr(expr)
{
    Type *type = ((Id *)lval)->getType();
    Type *exprType = expr->getType();
    SymbolEntry *se = lval->getSymbolEntry();
    bool flag = true;
    if (type->isInt())
    {
        if (((IntType *)type)->isConst())
        {
            fprintf(stderr, "cannot assign to variable \'%s\' with const-qualified type \'%s\'\n", ((IdentifierSymbolEntry *)se)->toStr().c_str(), type->toStr().c_str());
            flag = false;
        }
        else
        {
            ((IdentifierSymbolEntry *)se)->setValue(expr->getValue());
        }
    }
    else if (type->isFloat())
    {
        if (((FloatType *)type)->isConst())
        {
            fprintf(stderr, "cannot assign to variable \'%s\' with const-qualified type \'%s\'\n", ((IdentifierSymbolEntry *)se)->toStr().c_str(), type->toStr().c_str());
            flag = false;
        }
        else
        {
            ((IdentifierSymbolEntry *)se)->setValue(expr->getValue());
        }
    }
    else if (type->isArray())
    {
        fprintf(stderr, "array type \'%s\' is not assignable\n", type->toStr().c_str());
        flag = false;
    }
    if (flag)
    {
        if (type != exprType)
        {
            if (type->isInt() && exprType->isFloat())
            {
                ImplicitCastExpr *temp = new ImplicitCastExpr(expr, TypeSystem::intType);
                this->expr = temp;
            }
            else if (type->isFloat() && exprType->isInt())
            {
                ImplicitCastExpr *temp = new ImplicitCastExpr(expr, TypeSystem::floatType);
                this->expr = temp;
            }
            else
            {
                fprintf(stderr,
                        "cannot initialize a variable of type \'%s\' with an rvalue of type \'%s\'\n", type->toStr().c_str(), exprType->toStr().c_str());
            }
        }
    }
}

Type *Id::getType()
{
    SymbolEntry *se = this->getSymbolEntry();
    if (!se)
    {
        return TypeSystem::voidType;
    }
    Type *type = se->getType();
    if (!arrIdx)
    {
        return type;
    }
    else if (!type->isArray())
    {
        fprintf(stderr, "subscripted value is not an array\n");
        return TypeSystem::voidType;
    }
    else
    {
        ArrayType *temp1 = (ArrayType *)type;
        ExprNode *temp2 = arrIdx;
        while (!temp1->getElementType()->isInt() && !temp1->getElementType()->isFloat())
        {
            if (!temp2)
            {
                return temp1;
            }
            temp2 = (ExprNode *)(temp2->getNext());
            temp1 = (ArrayType *)(temp1->getElementType());
        }
        if (!temp2)
        {
            fprintf(stderr, "subscripted value is not an array\n");
            return temp1;
        }
        else if (temp2->getNext())
        {
            fprintf(stderr, "subscripted value is not an array\n");
            return TypeSystem::voidType;
        }

        return temp1->getElementType();
    }
}

void ExprNode::output(int level)
{
    std::string name, type;
    name = symbolEntry->toStr();
    type = symbolEntry->getType()->toStr();
    fprintf(yyout, "%*cconst string\ttype:%s\t%s\n", level, ' ', type.c_str(), name.c_str());
}

void Ast::output()
{
    fprintf(yyout, "program\n");
    if (root != nullptr)
    {
        root->output(4);
    }
}

void BinaryExpr::output(int level)
{
    std::string op_str;
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
    fprintf(yyout, "%*cBinaryExpr\top: %s\ttype: %s\n", level, ' ', op_str.c_str(), type->toStr().c_str());
    expr1->output(level + 4);
    expr2->output(level + 4);
}

double BinaryExpr::getValue()
{
    double value = 0;
    if (type->isFloat())
    {
        float val;
        float val1 = (float)(expr1->getValue());
        float val2 = (float)(expr2->getValue());

        switch (op)
        {
        case ADD:
            val = val1 + val2;
            break;
        case SUB:
            val = val1 - val2;
            break;
        case MUL:
            val = val1 * val2;
            break;
        case DIV:
            if (val2 != 0)
                val = val1 / val2;
            break;
        case MOD:
            val = (int)(val1) % (int)(val2);
            break;
        case AND:
            val = val1 && val2;
            break;
        case OR:
            val = val1 || val2;
            break;
        case LESS:
            val = val1 < val2;
            break;
        case LESSEQUAL:
            val = val1 <= val2;
            break;
        case GREATER:
            val = val1 > val2;
            break;
        case GREATEREQUAL:
            val = val1 >= val2;
            break;
        case EQUAL:
            val = val1 == val2;
            break;
        case NOTEQUAL:
            val = val1 != val2;
            break;
        }
        value = (double)val;
        return value;
    }

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
        if (expr2->getValue())
            value = expr1->getValue() / expr2->getValue();
        break;
    case MOD:
        if (expr2->getValue())
        {
            value = (int)(expr1->getValue()) % (int)(expr2->getValue());
        }
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

UnaryExpr::UnaryExpr(SymbolEntry *se, int op, ExprNode *expr) : ExprNode(se, UNARYEXPR), op(op), expr(expr)
{
    std::string op_str = op == UnaryExpr::NOT ? "!" : "-";
    if (expr->getType()->isVoid())
    {
        fprintf(stderr, "invalid operand of type \'void\' to unary \'opeartor%s\'\n", op_str.c_str());
    }
    if (op == UnaryExpr::NOT)
    {
        type = TypeSystem::intType;
        dst = new Operand(se);
        if (expr->isUnaryExpr())
        {
            UnaryExpr *ue = (UnaryExpr *)expr;
            if (ue->getOp() == UnaryExpr::NOT)
            {
                if (ue->getType() == TypeSystem::intType)
                {
                    ue->setType(TypeSystem::boolType);
                }
            }
        }
    }
    else if (op == UnaryExpr::SUB)
    {
        type = expr->getType();
        dst = new Operand(se);
        if (expr->isUnaryExpr())
        {
            UnaryExpr *ue = (UnaryExpr *)expr;
            if (ue->getOp() == UnaryExpr::NOT)
            {
                if (ue->getType() == TypeSystem::intType)
                {
                    ue->setType(TypeSystem::boolType);
                }
            }
        }
    }
};

void UnaryExpr::output(int level)
{
    std::string op_str;
    switch (op)
    {
    case NOT:
        op_str = "not";
        break;
    case SUB:
        op_str = "minus";
        break;
    }
    fprintf(yyout, "%*cUnaryExpr\top: %s\ttype: %s\n", level, ' ', op_str.c_str(), type->toStr().c_str());
    expr->output(level + 4);
}

double UnaryExpr::getValue()
{
    double value = 0;
    switch (op)
    {
    case NOT:
        value = !(expr->getValue());
        break;
    case SUB:
        value = -(expr->getValue());
        break;
    }
    return value;
}

void CallExpr::output(int level)
{
    std::string name, type;
    int scope;
    if (symbolEntry)
    {
        name = symbolEntry->toStr();
        type = symbolEntry->getType()->toStr();
        scope = dynamic_cast<IdentifierSymbolEntry *>(symbolEntry)->getScope();
        fprintf(yyout, "%*cCallExpr\tfunction name: %s\tscope: %d\ttype: %s\n", level, ' ', name.c_str(), scope, type.c_str());
        Node *temp = param;
        while (temp)
        {
            temp->output(level + 4);
            temp = temp->getNext();
        }
    }
}

void Constant::output(int level)
{
    std::string type, value;
    type = symbolEntry->getType()->toStr();
    value = symbolEntry->toStr();
    if (symbolEntry->getType()->isInt())
    {
        fprintf(yyout, "%*cIntegerLiteral\tvalue: %s\ttype: %s\n", level, ' ', value.c_str(), type.c_str());
    }
    else if (symbolEntry->getType()->isFloat())
    {
        fprintf(yyout, "%*cFloatLiteral\tvalue: %s\ttype: %s\n", level, ' ', value.c_str(), type.c_str());
    }
}

double Constant::getValue()
{
    return ((ConstantSymbolEntry *)symbolEntry)->getValue();
}

double Id::getValue()
{
    return ((IdentifierSymbolEntry *)symbolEntry)->getValue();
}

void Id::output(int level)
{
    std::string name, type;
    int scope;
    if (symbolEntry)
    {
        name = symbolEntry->toStr();
        type = symbolEntry->getType()->toStr();
        scope = dynamic_cast<IdentifierSymbolEntry *>(symbolEntry)->getScope();
        fprintf(yyout, "%*cId\tname: %s\tscope: %d\ttype: %s\n", level, ' ', name.c_str(), scope, type.c_str());
        if (arrIdx)
        {
            ExprNode *temp = arrIdx;
            int i = 0;
            while (temp)
            {
                temp->output(level + 4 + 4 * i++);
                temp = (ExprNode *)(temp->getNext());
            }
        }
    }
}

void InitValueListExpr::output(int level)
{
    std::string type;
    if (symbolEntry->getType())
    {
        type = symbolEntry->getType()->toStr();
    }
    fprintf(yyout, "%*cInitValueListExpr\ttype: %s\n", level, ' ', type.c_str());
    Node *temp = expr;
    while (temp)
    {
        temp->output(level + 4);
        temp = temp->getNext();
    }
}

void InitValueListExpr::addExpr(ExprNode *expr)
{
    if (this->expr == nullptr)
    {
        assert(childCnt == 0);
        childCnt++;
        this->expr = expr;
    }
    else
    {
        childCnt++;
        this->expr->setNext(expr);
    }
}

bool InitValueListExpr::isFull()
{
    ArrayType *type = (ArrayType *)(this->symbolEntry->getType());
    return childCnt == type->getLength();
}

void InitValueListExpr::fill()
{
    Type *type = ((ArrayType *)(this->getType()))->getElementType();
    if (type->isArray())
    {
        while (!isFull())
        {
            this->addExpr(new InitValueListExpr(new ConstantSymbolEntry(type)));
        }
        ExprNode *temp = expr;
        while (temp)
        {
            ((InitValueListExpr *)temp)->fill();
            temp = (ExprNode *)(temp->getNext());
        }
    }
    if (type->isInt() || type->isFloat())
    {
        while (!isFull())
        {
            this->addExpr(new Constant(new ConstantSymbolEntry(type, 0)));
        }
        return;
    }
}

void ImplicitCastExpr::output(int level)
{
    fprintf(yyout, "%*cImplicitCastExpr\ttype: %s to %s\n", level, ' ', expr->getType()->toStr().c_str(), type->toStr().c_str());
    this->expr->output(level + 4);
}

void CompoundStmt::output(int level)
{
    fprintf(yyout, "%*cCompoundStmt\n", level, ' ');
    if (stmt)
    {
        stmt->output(level + 4);
    }
}

void SeqNode::output(int level)
{
    stmt1->output(level);
    stmt2->output(level);
}

DeclStmt::DeclStmt(Id *id, ExprNode *expr) : id(id)
{
    if (expr)
    {
        this->expr = expr;
        if (expr->isInitValueListExpr())
        {
            ((InitValueListExpr *)(this->expr))->fill();
        }
        else
        {
            Type *idType = id->getType();
            Type *exprType = expr->getType();
            if ((idType->isFloat() && exprType->isInt()) || (idType->isInt() && exprType->isFloat()))
            {
                ImplicitCastExpr *temp = new ImplicitCastExpr(expr, idType);
                this->expr = temp;
            }
        }
    }
}

void DeclStmt::output(int level)
{
    fprintf(yyout, "%*cDeclStmt\n", level, ' ');
    id->output(level + 4);
    if (expr)
    {
        expr->output(level + 4);
    }
    if (this->getNext())
    {
        this->getNext()->output(level);
    }
}

void BlankStmt::output(int level)
{
    fprintf(yyout, "%*cBlankStmt\n", level, ' ');
}

IfStmt::IfStmt(ExprNode *cond, StmtNode *thenStmt) : cond(cond), thenStmt(thenStmt)
{
    if (cond->getType()->isInt() && cond->getType()->getSize() == 32)
    {
        ImplicitCastExpr *temp = new ImplicitCastExpr(cond);
        this->cond = temp;
    }
    else if (cond->getType()->isFloat())
    {
        SymbolEntry *zero = new ConstantSymbolEntry(TypeSystem::floatType, 0);
        SymbolEntry *temp = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        BinaryExpr *cmpZero = new BinaryExpr(temp, BinaryExpr::NOTEQUAL, cond, new Constant(zero));
        this->cond = cmpZero;
    }
};

void IfStmt::output(int level)
{
    fprintf(yyout, "%*cIfStmt\n", level, ' ');
    cond->output(level + 4);
    thenStmt->output(level + 4);
}

void IfElseStmt::output(int level)
{
    fprintf(yyout, "%*cIfElseStmt\n", level, ' ');
    cond->output(level + 4);
    thenStmt->output(level + 4);
    elseStmt->output(level + 4);
}

void WhileStmt::output(int level)
{
    fprintf(yyout, "%*cWhileStmt\n", level, ' ');
    cond->output(level + 4);
    stmt->output(level + 4);
}
void BreakStmt::output(int level)
{
    fprintf(yyout, "%*cBreakStmt\n", level, ' ');
}

void ContinueStmt::output(int level)
{
    fprintf(yyout, "%*cContinueStmt\n", level, ' ');
}

void ReturnStmt::output(int level)
{
    fprintf(yyout, "%*cReturnStmt\n", level, ' ');
    if (retValue != nullptr)
    {
        retValue->output(level + 4);
    }
}

void AssignStmt::output(int level)
{
    fprintf(yyout, "%*cAssignStmt\n", level, ' ');
    lval->output(level + 4);
    expr->output(level + 4);
}

void ExprStmt::output(int level)
{
    fprintf(yyout, "%*cExprStmt\n", level, ' ');
    expr->output(level + 4);
}

void FunctionDef::output(int level)
{
    std::string name, type;
    name = se->toStr();
    type = se->getType()->toStr();
    fprintf(yyout, "%*cFunctionDefine\tfunction name: %s\ttype: %s\n", level, ' ', name.c_str(), type.c_str());
    if (decl)
    {
        decl->output(level + 4);
    }
    stmt->output(level + 4);
}

void ImplicitCastExpr::genCode()
{
    expr->genCode();
    BasicBlock *bb = builder->getInsertBB();

    if (type == TypeSystem::boolType)
    {
        Function *func = bb->getParent();
        BasicBlock *trueBB = new BasicBlock(func);
        BasicBlock *tempbb = new BasicBlock(func);
        BasicBlock *falseBB = new BasicBlock(func);

        new CmpInstruction(CmpInstruction::NE, this->dst, this->expr->getOperand(), new Operand(new ConstantSymbolEntry(TypeSystem::intType, 0)), bb);
        this->trueList().push_back(new CondBrInstruction(trueBB, tempbb, this->dst, bb));
        this->falseList().push_back(new UncondBrInstruction(falseBB, tempbb));
    }
    else if (type->isInt())
    {
        new FptosiInstruction(dst, this->expr->getOperand(), bb);
    }
    else if (type->isFloat())
    {
        new SitofpInstruction(dst, this->expr->getOperand(), bb);
    }
    else
    {
        assert(false);
    }
}

double ImplicitCastExpr::getValue()
{
    if (type == TypeSystem::boolType)
    {
        return -1;
    }
    double temp = expr->getValue();

    Type *srcType = expr->getType();

    if (type->isInt() && srcType->isFloat())
    {
        float temp1 = (float)temp;
        int res = (int)temp1;
        temp = (double)res;
        return temp;
    }
    else if (type->isFloat() && srcType->isInt())
    {
        int temp1 = (int)temp;
        float res = (float)temp1;
        temp = (double)res;
        return temp;
    }
    else
    {
        return -1;
    }
}