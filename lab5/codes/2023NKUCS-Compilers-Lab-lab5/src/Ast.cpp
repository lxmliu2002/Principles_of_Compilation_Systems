#include "Ast.h"
#include "SymbolTable.h"
#include "Unit.h"
#include "Instruction.h"
#include "IRBuilder.h"
#include <string>
#include "Type.h"

using namespace std;

extern FILE *yyout;
int Node::counter = 0;
IRBuilder *Node::builder = nullptr;
bool isInwhile = false;
Type *retType = nullptr;
bool hasRet = false;

Node::Node()
{
    seq = counter++;
}

void Node::backPatch(std::vector<Instruction *> &list, BasicBlock *bb)
{
    for (auto &inst : list)
    {
        if (inst->isCond())
            dynamic_cast<CondBrInstruction *>(inst)->setTrueBranch(bb);
        else if (inst->isUncond())
            dynamic_cast<UncondBrInstruction *>(inst)->setBranch(bb);
    }
}

void Node::setNext(Node *node)
{
    Node *temp = this;
    while (temp->getNext())
    {
        temp = temp->getNext();
    }
    temp->next = node;
}

Node *Node::getNext()
{
    return next;
}

std::vector<Instruction *> Node::merge(std::vector<Instruction *> &list1, std::vector<Instruction *> &list2)
{
    std::vector<Instruction *> res(list1);
    res.insert(res.end(), list2.begin(), list2.end());
    return res;
}

void ExprNode::toBool(Function *func)
{
    TemporarySymbolEntry *se = new TemporarySymbolEntry(TypeSystem::boolType, identifiers->getLabel());
    Operand *tmp = new Operand(se);
    BasicBlock *bb = builder->getInsertBB();
    BasicBlock *trueBB = new BasicBlock(func);
    BasicBlock *falseBB = new BasicBlock(func);
    BasicBlock *tempBB = new BasicBlock(func);
    new ToBoolInstruction(tmp, dst, bb);
    this->true_list.push_back(new CondBrInstruction(trueBB, tempBB, tmp, bb));
    this->false_list.push_back(new UncondBrInstruction(falseBB, tempBB));
    this->dst = tmp;
}

int BinaryExpr::getInt()
{
    if (symbolEntry->tryGetValue())
        return symbolEntry->getInt();
    int value1, value2;
    Type *type1, *type2;
    type1 = expr1->getSymPtr()->getType();
    type2 = expr2->getSymPtr()->getType();
    if (type1->isPtr())
        type1 = ((PointerType *)type1)->getValueType();
    if (type2->isPtr())
        type2 = ((PointerType *)type2)->getValueType();
    if (type1->isInt())
        value1 = expr1->getInt();
    else if (type1->isFloat())
        value1 = expr2->getFloat();
    else
        return 0;
    if (type2->isInt())
        value2 = expr1->getInt();
    else if (type2->isFloat())
        value2 = expr2->getFloat();
    else
        return 0;
    switch (op)
    {
    case ADD:
        symbolEntry->setValue(value1 + value2);
        break;
    case SUB:
        symbolEntry->setValue(value1 - value2);
        break;
    case MUL:
        symbolEntry->setValue(value1 * value2);
        break;
    case DIV:
        symbolEntry->setValue(value1 / value2);
        break;
    case MOD:
        symbolEntry->setValue(value1 % value2);
        break;
    case AND:
        symbolEntry->setValue(value1 && value2);
        break;
    case OR:
        symbolEntry->setValue(value1 || value2);
        break;
    case LESS:
        symbolEntry->setValue(value1 < value2);
        break;
    case LESSEQUAL:
        symbolEntry->setValue(value1 <= value2);
        break;
    case GREATER:
        symbolEntry->setValue(value1 > value2);
        break;
    case GREATEREQUAL:
        symbolEntry->setValue(value1 >= value2);
        break;
    case EQUAL:
        symbolEntry->setValue(value1 == value2);
        break;
    case NOTEQUAL:
        symbolEntry->setValue(value1 != value2);
        break;
    default:
        return 0;
    }
    if (symbolEntry->getType()->isInt())
        return symbolEntry->getInt();
    else
        return symbolEntry->getFloat();
}

float BinaryExpr::getFloat()
{
    float value1, value2;
    Type *type1, *type2;
    type1 = expr1->getSymPtr()->getType();
    if (type1->isPtr())
        type1 = ((PointerType *)type1)->getValueType();
    type2 = expr2->getSymPtr()->getType();
    if (type2->isPtr())
        type2 = ((PointerType *)type2)->getValueType();
    if (type1->isInt())
        value1 = expr1->getInt();
    else if (type1->isFloat())
        value1 = expr2->getFloat();
    else
        return 0;
    if (type2->isInt())
        value2 = expr1->getInt();
    else if (type2->isFloat())
        value2 = expr2->getFloat();
    else
        return 0;
    switch (op)
    {
    case ADD:
        symbolEntry->setValue(value1 + value2);
        break;
    case SUB:
        symbolEntry->setValue(value1 - value2);
        break;
    case MUL:
        symbolEntry->setValue(value1 * value2);
        break;
    case DIV:
        symbolEntry->setValue(value1 / value2);
        break;
    case AND:
        symbolEntry->setValue(value1 && value2);
        break;
    case OR:
        symbolEntry->setValue(value1 || value2);
        break;
    case LESS:
        symbolEntry->setValue(value1 < value2);
        break;
    case LESSEQUAL:
        symbolEntry->setValue(value1 <= value2);
        break;
    case GREATER:
        symbolEntry->setValue(value1 > value2);
        break;
    case GREATEREQUAL:
        symbolEntry->setValue(value1 >= value2);
        break;
    case EQUAL:
        symbolEntry->setValue(value1 == value2);
        break;
    case NOTEQUAL:
        symbolEntry->setValue(value1 != value2);
        break;
    default:
        return 0;
    }
    if (symbolEntry->getType()->isInt())
        return symbolEntry->getInt();
    else
        return symbolEntry->getFloat();
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
    // set the insert point to the entry basicblock of this function.
    builder->setInsertBB(entry);
    DeclStmt *curr = FuncDefParams;
    while (curr != nullptr)
    {
        func->addParam(curr->getId()->getSymPtr());
        curr->genCode();
        curr = (DeclStmt *)curr->getNext();
    }
    stmt->genCode();

    /**
     * Construct control flow graph. You need do set successors and predecessors for each basic block.
     * Todo
     */
    // 建立块之间的关系
    for (auto bb : *func)
    {
        Instruction *curr = bb->begin();
        bool isDelete = false;
        for (; curr != bb->end();)
        {
            if (isDelete)
            {
                Instruction *next = curr->getNext();
                bb->remove(curr);
                delete curr;
                curr = next;
                continue;
            }
            if (curr->isCond() && curr != bb->rbegin())
            {
                Instruction *next = curr->getNext();
                bb->remove(curr);
                delete curr;
                curr = next;
                continue;
            }
            if (curr->isRet() || curr->isUncond())
            {
                isDelete = true;
            }
            curr = curr->getNext();
        }
        Instruction *last = bb->rbegin();
        if (last->isUncond())
        {
            UncondBrInstruction *ucbr = dynamic_cast<UncondBrInstruction *>(last);
            BasicBlock *bb_branch = ucbr->getBranch();
            bb->addSucc(bb_branch);
            bb_branch->addPred(bb);
        }
        else if (last->isCond())
        {
            CondBrInstruction *cbr = dynamic_cast<CondBrInstruction *>(last);
            BasicBlock *tb = cbr->getTrueBranch(), *fb = cbr->getFalseBranch();
            bb->addSucc(tb);
            bb->addSucc(fb);
            tb->addPred(bb);
            fb->addPred(bb);
        }
        else if (!last->isRet())
        {
            // 最后一条语句不是跳转也不是ret，则加上ret
            new RetInstruction(nullptr, bb);
        }
        // 如果块中没有指令，增加一个ret
        // if(bb->begin() == bb->end())
        // {
        //     new RetInstruction(nullptr, bb);
        // }
    }
}

void BinaryExpr::genCode()
{
    BasicBlock *bb = builder->getInsertBB();
    Function *func = bb->getParent();
    if (op == AND)
    {
        BasicBlock *trueBB = new BasicBlock(func); // if the result of lhs is true, jump to the trueBB.
        expr1->genCode();
        if (expr1->getOperand()->getType() != TypeSystem::boolType)
            expr1->toBool(func);
        backPatch(expr1->trueList(), trueBB);
        builder->setInsertBB(trueBB); // set the insert point to the trueBB so that intructions generated by expr2 will be inserted into it.
        expr2->genCode();
        if (expr2->getOperand()->getType() != TypeSystem::boolType)
            expr2->toBool(func);
        true_list = expr2->trueList();
        false_list = merge(expr1->falseList(), expr2->falseList());
    }
    else if (op == OR)
    {
        // Todo
        BasicBlock *falseBB = new BasicBlock(func);
        expr1->genCode();
        if (expr1->getOperand()->getType() != TypeSystem::boolType)
            expr1->toBool(func);
        backPatch(expr1->falseList(), falseBB);
        builder->setInsertBB(falseBB); // set the insert point to the trueBB so that intructions generated by expr2 will be inserted into it.
        expr2->genCode();
        if (expr2->getOperand()->getType() != TypeSystem::boolType)
            expr2->toBool(func);
        false_list = expr2->falseList();
        true_list = merge(expr1->trueList(), expr2->trueList());
    }
    else if (op >= LESS && op <= NOTEQUAL)
    {
        // Todo
        expr1->genCode();
        expr2->genCode();
        Operand *src1 = expr1->getOperand();
        Operand *src2 = expr2->getOperand();
        int opcode;
        switch (op)
        {
        case LESS:
            opcode = CmpInstruction::L;
            break;
        case LESSEQUAL:
            opcode = CmpInstruction::LE;
            break;
        case GREATER:
            opcode = CmpInstruction::G;
            break;
        case GREATEREQUAL:
            opcode = CmpInstruction::GE;
            break;
        case EQUAL:
            opcode = CmpInstruction::E;
            break;
        case NOTEQUAL:
            opcode = CmpInstruction::NE;
            break;
        default:
            opcode = -1;
            break;
        }
        if (src1->getType() != src2->getType())
        {
            if (src1->getType() != TypeSystem::boolType && src2->getType() == TypeSystem::boolType)
            {
                Operand *srcNew = new Operand(new TemporarySymbolEntry(src1->getType(), SymbolTable::getLabel()));
                new TypeConverInstruction(srcNew, src2, bb);
                src2 = srcNew;
            }
            else if (src2->getType() != TypeSystem::boolType && src1->getType() == TypeSystem::boolType)
            {
                Operand *srcNew = new Operand(new TemporarySymbolEntry(src2->getType(), SymbolTable::getLabel()));
                new TypeConverInstruction(srcNew, src1, bb);
                src1 = srcNew;
            }
            if(src1->getType()->isFloat() && expr2->getSymPtr()->isConstant())
            {
                
            }
        }
        new CmpInstruction(opcode, dst, src1, src2, bb);
        BasicBlock *trueBB = new BasicBlock(func);
        BasicBlock *falseBB = new BasicBlock(func);
        BasicBlock *tempBB = new BasicBlock(func);
        this->true_list.push_back(new CondBrInstruction(trueBB, tempBB, dst, bb));
        this->false_list.push_back(new UncondBrInstruction(falseBB, tempBB));
    }
    else if (op >= ADD && op <= MOD)
    {
        expr1->genCode();
        expr2->genCode();
        Operand *src1 = expr1->getOperand();
        Operand *src2 = expr2->getOperand();
        int opcode;
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
        default:
            opcode = -1;
            break;
        }
        if (dst->getType() != src1->getType())
        {
            Operand *srcNew = new Operand(new TemporarySymbolEntry(dst->getType(), SymbolTable::getLabel()));
            new TypeConverInstruction(srcNew, src1, bb);
            src1 = srcNew;
        }
        if (dst->getType() != src2->getType())
        {
            Operand *srcNew = new Operand(new TemporarySymbolEntry(dst->getType(), SymbolTable::getLabel()));
            new TypeConverInstruction(srcNew, src2, bb);
            src2 = srcNew;
        }
        new BinaryInstruction(opcode, dst, src1, src2, bb);
    }
}

void UnaryExpr::genCode()
{
    BasicBlock *bb = builder->getInsertBB();
    Function *func = bb->getParent();
    expr->genCode();
    int opcode;

    switch (op)
    {
    case ADD:
        opcode = UnaryInstruction::ADD;
        break;
    case SUB:
        opcode = UnaryInstruction::SUB;
        break;
    case NOT:
        opcode = UnaryInstruction::NOT;
        if (expr->getOperand()->getType() != TypeSystem::boolType)
            expr->toBool(func);
        break;
    default:
        opcode = -1;
        break;
    }
    Operand *src = expr->getOperand();
    if (dst->getType() != src->getType())
    {
        Operand *srcNew = new Operand(new TemporarySymbolEntry(dst->getType(), SymbolTable::getLabel()));
        new TypeConverInstruction(srcNew, src, bb);
        src = srcNew;
    }
    new UnaryInstruction(opcode, dst, src, bb);
    BasicBlock *trueBB = new BasicBlock(func);
    BasicBlock *falseBB = new BasicBlock(func);
    BasicBlock *tempBB = new BasicBlock(func);
    this->true_list.push_back(new CondBrInstruction(trueBB, tempBB, dst, bb));
    this->false_list.push_back(new UncondBrInstruction(falseBB, tempBB));
}

void FuncCallExp::genCode()
{
    std::vector<Operand *> params;
    ExprNode *curr = param;
    BasicBlock *bb = builder->getInsertBB();
    while (curr != nullptr)
    {
        curr->genCode();
        params.push_back(curr->getOperand());
        curr = (ExprNode *)curr->getNext();
    }
    new FuncCallInstruction(funcSym, dst, params, bb);
}

void Constant::genCode()
{
    // we don't need to generate code.
}

void Id::genCode()
{
    BasicBlock *bb = builder->getInsertBB();
    Operand *addr = dynamic_cast<IdentifierSymbolEntry *>(symbolEntry)->getAddr();
    new LoadInstruction(dst, addr, bb);
}

void IfStmt::genCode()
{
    Function *func;
    BasicBlock *then_bb, *end_bb;

    func = builder->getInsertBB()->getParent();
    then_bb = new BasicBlock(func);
    end_bb = new BasicBlock(func);

    cond->genCode();
    if (cond->getOperand()->getType() != TypeSystem::boolType)
        cond->toBool(func);
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
    // Todo
    Function *func;
    BasicBlock *then_bb, *else_bb, *end_bb;

    func = builder->getInsertBB()->getParent();
    then_bb = new BasicBlock(func);
    else_bb = new BasicBlock(func);
    end_bb = new BasicBlock(func);

    cond->genCode();
    if (cond->getOperand()->getType() != TypeSystem::boolType)
        cond->toBool(func);
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

void WhileStmt::genCode()
{
    Function *func;
    BasicBlock *while_bb, *then_bb, *end_bb;

    func = builder->getInsertBB()->getParent();
    while_bb = new BasicBlock(func);
    then_bb = new BasicBlock(func);
    end_bb = new BasicBlock(func);

    new UncondBrInstruction(while_bb, builder->getInsertBB());
    builder->setInsertBB(while_bb);
    cond->genCode();
    if (cond->getOperand()->getType() != TypeSystem::boolType)
        cond->toBool(func);
    backPatch(cond->trueList(), then_bb);
    backPatch(cond->falseList(), end_bb);
    while_bb = builder->getInsertBB();

    builder->setInsertBB(then_bb);
    stmt->genCode();
    backPatch(builder->getWhileList(true), while_bb);
    backPatch(builder->getWhileList(false), end_bb);
    then_bb = builder->getInsertBB();

    new UncondBrInstruction(while_bb, then_bb);

    builder->setInsertBB(end_bb);
}

void BreakStmt::genCode()
{
    BasicBlock *bb = builder->getInsertBB();
    Function *func = bb->getParent();
    BasicBlock *breakBB = new BasicBlock(func);
    new UncondBrInstruction(breakBB, bb);
    // builder->setInsertBB(breakBB);
    BasicBlock *tempBB = new BasicBlock(func);
    builder->addWhileList(new UncondBrInstruction(tempBB, breakBB), false);
    // builder->setInsertBB(bb);
}

void ContinueStmt::genCode()
{
    BasicBlock *bb = builder->getInsertBB();
    Function *func = bb->getParent();
    BasicBlock *continueBB = new BasicBlock(func);
    new UncondBrInstruction(continueBB, bb);
    // builder->setInsertBB(continueBB);
    BasicBlock *tempBB = new BasicBlock(func);
    builder->addWhileList(new UncondBrInstruction(tempBB, continueBB), true);
    // builder->setInsertBB(bb);
}

void CompoundStmt::genCode()
{
    // Todo
    if (stmt)
        stmt->genCode();
}

void SeqNode::genCode()
{
    // Todo
    // stmt1->genCode();
    // stmt2->genCode();
    for (auto p : stmts)
    {
        p->genCode();
    }
}

void ExprStmt::genCode()
{
    expr->genCode();
}

void DeclStmt::genCode()
{
    if (next != nullptr)
        next->genCode();
    IdentifierSymbolEntry *se = dynamic_cast<IdentifierSymbolEntry *>(id->getSymPtr());
    if (se->isGlobal())
    {
        Operand *addr;
        SymbolEntry *addr_se;
        addr_se = new IdentifierSymbolEntry(*se);
        addr_se->setType(new PointerType(se->getType()));
        addr = new Operand(addr_se);
        se->setAddr(addr);
        if (expr == nullptr)
        {
            builder->getUnit()->insertGlobalVar(new GlobalVarDefInstruction(addr, nullptr, builder->getInsertBB()));
        }
        else
        {
            ConstantSymbolEntry *valueEntry = (ConstantSymbolEntry *)expr->getSymPtr();
            builder->getUnit()->insertGlobalVar(new GlobalVarDefInstruction(addr, valueEntry, builder->getInsertBB()));
        }
    }
    else if (se->isLocal())
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
        alloca = new AllocaInstruction(addr, se); // allocate space for local id in function stack.
        entry->insertFront(alloca);               // allocate instructions should be inserted into the begin of the entry block.
        se->setAddr(addr);                        // set the addr operand in symbol entry so that we can use it in subsequent code generation.
        if (expr != nullptr)
        {
            if (!isParam)
            {
                expr->genCode();
                new StoreInstruction(addr, expr->getOperand(), builder->getInsertBB());
            }
            else
            {
                new StoreInstruction(addr, ((IdentifierSymbolEntry *)expr->getSymPtr())->getAddr(), builder->getInsertBB());
            }
        }
    }
    else if (se->isParam())
    {
        Operand *addr;
        SymbolEntry *addr_se;
        addr_se = new TemporarySymbolEntry(se->getType(), SymbolTable::getLabel());
        addr = new Operand(addr_se);
        se->setAddr(addr);
    }
}

void ReturnStmt::genCode()
{
    // Todo
    BasicBlock *bb = builder->getInsertBB();
    Operand *op = nullptr;
    if (retValue != nullptr)
    {
        retValue->genCode();
        op = retValue->getOperand();
    }
    new RetInstruction(op, bb);
}

void AssignStmt::genCode()
{
    BasicBlock *bb = builder->getInsertBB();
    expr->genCode();
    Operand *addr = dynamic_cast<IdentifierSymbolEntry *>(lval->getSymPtr())->getAddr();
    Operand *src = expr->getOperand();
    /***
     * We haven't implemented array yet, the lval can only be ID. So we just store the result of the `expr` to the addr of the id.
     * If you want to implement array, you have to caculate the address first and then store the result into it.
     */
    if (((PointerType *)addr->getType())->getValueType() != src->getType())
    {
        Operand *srcNew = new Operand(new TemporarySymbolEntry(addr->getType(), SymbolTable::getLabel()));
        new TypeConverInstruction(srcNew, src, bb);
        src = srcNew;
    }
    new StoreInstruction(addr, src, bb);
}

void Ast::typeCheck()
{
    if (root != nullptr)
        root->typeCheck();
}

void FunctionDef::typeCheck()
{
    // Todo
    hasRet = false;
    retType = ((FunctionType *)se->getType())->getRetType();
    stmt->typeCheck();
    if (!hasRet && !retType->isVoid())
    {
        fprintf(stderr, "function must has returnStmt\n");
        exit(-1);
    }
}

void BinaryExpr::typeCheck()
{
    // Todo
    expr1->typeCheck();
    expr2->typeCheck();
    Type *type1 = expr1->getOperand()->getType();
    Type *type2 = expr2->getOperand()->getType();
    if (type1->isFunc())
        type1 = ((FunctionType *)type1)->getRetType();
    if (type2->isFunc())
        type2 = ((FunctionType *)type2)->getRetType();
    if (type1->isVoid() || type2->isVoid())
    {
        fprintf(stderr, "type void cannot use in BinaryExpr\n");
        exit(-1);
    }
}

void Constant::typeCheck()
{
    // Todo
}

void Id::typeCheck()
{
    // Todo
    if (symbolEntry == nullptr)
    {
        fprintf(stderr, "undefined id\n");
        exit(-1);
    }
}

void IfStmt::typeCheck()
{
    // Todo
    cond->typeCheck();
    // cond->getSymPtr()->setType(TypeSystem::boolType);
    thenStmt->typeCheck();
}

void IfElseStmt::typeCheck()
{
    // Todo
    cond->typeCheck();
    // cond->getSymPtr()->setType(TypeSystem::boolType);
    thenStmt->typeCheck();
    elseStmt->typeCheck();
}

void CompoundStmt::typeCheck()
{
    // Todo
    stmt->typeCheck();
}

void SeqNode::typeCheck()
{
    // Todo
    // stmt1->typeCheck();
    // stmt2->typeCheck();
    for (auto p : stmts)
    {
        p->typeCheck();
    }
}

void DeclStmt::typeCheck()
{
    // Todo
    // 重定义在语法分析阶段实现
    if (expr != nullptr)
        expr->typeCheck();
}

void ReturnStmt::typeCheck()
{
    // Todo
    hasRet = true;
    if (retValue == nullptr)
    {
        if (!retType->isVoid())
            fprintf(stderr, "return value cannot be void\n");
        return;
    }
    retValue->typeCheck();
    if (!Type::isValid(retType, retValue->getOperand()->getType()))
    {
        fprintf(stderr, "return type error\n");
        exit(1);
    }
    if (retType->isVoid() && retValue != nullptr)
    {
        fprintf(stderr, "void function cannot return value\n");
        exit(1);
    }
}

void AssignStmt::typeCheck()
{
    // Todo
    lval->typeCheck();
    expr->typeCheck();
    if (!Type::isValid(lval->getSymPtr()->getType(), expr->getOperand()->getType()))
    {
        fprintf(stderr, "assign type error\n");
        exit(-1);
    }
}

void UnaryExpr::typeCheck()
{
    expr->typeCheck();
}

void FuncCallExp::typeCheck()
{
    FunctionType *t = (FunctionType *)funcSym->getType();
    ExprNode *curr = param;
    int i = 0;
    while (curr != nullptr)
    {
        curr->typeCheck();
        Type *p = t->getParamsType(i);
        if (p == nullptr)
        {
            fprintf(stderr, "FuncCallExp param number error\n");
            exit(-1);
        }
        Type *currType = curr->getSymPtr()->getType();
        if (currType->isFunc())
            currType = ((FunctionType *)currType)->getRetType();
        if (!Type::isValid(p, currType))
        {
            fprintf(stderr, "FuncCallExp param type error\n");
            exit(-1);
        }
        i++;
        curr = (ExprNode *)curr->getNext();
    }
    if (t->getParamsType(i) != nullptr)
    {
        fprintf(stderr, "FuncCallExp param number error\n");
        exit(-1);
    }
}

void ExprStmt::typeCheck()
{
    expr->typeCheck();
}

void WhileStmt::typeCheck()
{
    cond->typeCheck();
    isInwhile = true;
    stmt->typeCheck();
    isInwhile = false;
}

void BreakStmt::typeCheck()
{
    if (!isInwhile)
    {
        fprintf(stderr, "break should in whileStmt\n");
        exit(-1);
    }
}

void ContinueStmt::typeCheck()
{
    if (!isInwhile)
    {
        fprintf(stderr, "continue should in whileStmt\n");
        exit(-1);
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
    fprintf(yyout, "%*cBinaryExpr\top: %s\n", level, ' ', op_str.c_str());
    expr1->output(level + 4);
    expr2->output(level + 4);
}

void Ast::output()
{
    fprintf(yyout, "program\n");
    if (root != nullptr)
        root->output(4);
}

void Constant::output(int level)
{
    std::string type, value;
    type = symbolEntry->getType()->toStr();
    value = symbolEntry->toStr();
    fprintf(yyout, "%*cIntegerLiteral\tvalue: %s\ttype: %s\n", level, ' ',
            value.c_str(), type.c_str());
}

void Id::output(int level)
{
    std::string name, type;
    int scope;
    name = symbolEntry->toStr();
    type = symbolEntry->getType()->toStr();
    scope = dynamic_cast<IdentifierSymbolEntry *>(symbolEntry)->getScope();
    if (dynamic_cast<IdentifierSymbolEntry *>(symbolEntry)->isConst())
        fprintf(yyout, "%*cId\tname: %s\tscope: %d\ttype: const %s\n", level, ' ',
                name.c_str(), scope, type.c_str());
    else
        fprintf(yyout, "%*cId\tname: %s\tscope: %d\ttype: %s\n", level, ' ',
                name.c_str(), scope, type.c_str());
}

void FuncCallExp::output(int level)
{
    string name, type;
    int scope;
    name = funcSym->toStr();
    type = funcSym->getType()->toStr();
    scope = dynamic_cast<IdentifierSymbolEntry *>(funcSym)->getScope();
    fprintf(yyout, "%*cFuncCallExpr\tfunction name: %s\tscope: %d\ttype: %s\n",
            level, ' ', name.c_str(), scope, type.c_str());
    Node *temp = param;
    while (temp)
    {
        temp->output(level + 4);
        temp = temp->getNext();
    }
}

void ExprStmt::output(int level)
{
    fprintf(yyout, "%*cExprStmt\n", level, ' ');
    expr->output(level + 4);
}

void UnaryExpr::output(int level)
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
    fprintf(yyout, "%*cUnaryExpr\top: %s\ttype: %s\n", level, ' ', op_str.c_str(), symbolEntry->getType()->toStr().c_str());
    expr->output(level + 4);
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
    // stmt1->output(level);
    // stmt2->output(level);
    for (auto p : stmts)
    {
        p->output(level);
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

void EmptyStmt::output(int level)
{
    fprintf(yyout, "%*cEmptyStmt\n", level, ' ');
}

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
        retValue->output(level + 4);
}

void AssignStmt::output(int level)
{
    fprintf(yyout, "%*cAssignStmt\n", level, ' ');
    lval->output(level + 4);
    expr->output(level + 4);
}

void FunctionDef::output(int level)
{
    std::string name, type;
    name = se->toStr();
    type = se->getType()->toStr();
    fprintf(yyout, "%*cFunctionDefine function name: %s type: %s\n", level, ' ',
            name.c_str(), type.c_str());
    if (FuncDefParams)
    {
        FuncDefParams->output(level + 4);
    }
    stmt->output(level + 4);
}
