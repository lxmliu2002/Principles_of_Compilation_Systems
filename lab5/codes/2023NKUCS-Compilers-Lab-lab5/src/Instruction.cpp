#include "Instruction.h"
#include "BasicBlock.h"
#include <iostream>
#include "Function.h"
#include "Type.h"
extern FILE *yyout;

Instruction::Instruction(unsigned instType, BasicBlock *insert_bb)
{
    prev = next = this;
    opcode = -1;
    this->instType = instType;
    if (insert_bb != nullptr)
    {
        insert_bb->insertBack(this);
        parent = insert_bb;
    }
}

Instruction::~Instruction()
{
    parent->remove(this);
}

BasicBlock *Instruction::getParent()
{
    return parent;
}

void Instruction::setParent(BasicBlock *bb)
{
    parent = bb;
}

void Instruction::setNext(Instruction *inst)
{
    next = inst;
}

void Instruction::setPrev(Instruction *inst)
{
    prev = inst;
}

Instruction *Instruction::getNext()
{
    return next;
}

Instruction *Instruction::getPrev()
{
    return prev;
}

BinaryInstruction::BinaryInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2, BasicBlock *insert_bb) : Instruction(BINARY, insert_bb)
{
    this->opcode = opcode;
    operands.push_back(dst);
    operands.push_back(src1);
    operands.push_back(src2);
    dst->setDef(this);
    src1->addUse(this);
    src2->addUse(this);
}

BinaryInstruction::~BinaryInstruction()
{
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
    operands[2]->removeUse(this);
}

void BinaryInstruction::output() const
{
    std::string s1, s2, s3, op, type;
    s1 = operands[0]->toStr();
    s2 = operands[1]->toStr();
    s3 = operands[2]->toStr();
    type = operands[0]->getType()->toStr();
    bool isfloat = false;
    if (operands[1]->getType()->isFloat() || operands[2]->getType()->isFloat())
        isfloat = true;
    if(isfloat && operands[1]->getType()->isInt() && operands[1]->getSymPtr()->isConstant())
    {
        s2 += ".0";
    }
    if(isfloat && operands[2]->getType()->isInt() && operands[2]->getSymPtr()->isConstant())
    {
        s3 += ".0";
    }
    switch (opcode)
    {
    case ADD:
        if (isfloat)
            op = "fadd";
        else
            op = "add";
        break;
    case SUB:
        if (isfloat)
            op = "fsub";
        else
            op = "sub";
        break;
    case MUL:
        if (isfloat)
            op = "fmul";
        else
            op = "mul";
        break;
    case DIV:
        if (isfloat)
            op = "fdiv";
        else
            op = "sdiv";
        break;
    case MOD:
        if (isfloat)
            op = "frem";
        else
            op = "srem";
    default:
        break;
    }
    fprintf(yyout, "  %s = %s %s %s, %s\n", s1.c_str(), op.c_str(), type.c_str(), s2.c_str(), s3.c_str());
}

UnaryInstruction::UnaryInstruction(unsigned opcode, Operand *dst, Operand *src, BasicBlock *insert_bb) : Instruction(BINARY, insert_bb)
{
    this->opcode = opcode;
    operands.push_back(dst);
    operands.push_back(src);
    dst->setDef(this);
    src->addUse(this);
}

UnaryInstruction::~UnaryInstruction()
{
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
}

void UnaryInstruction::output() const
{
    std::string s1, s2, s3, op, type;
    s1 = operands[0]->toStr();
    s2 = operands[1]->toStr();
    type = operands[0]->getType()->toStr();
    bool isfloat = false;
    if (operands[1]->getType()->isFloat())
        isfloat = true;
<<<<<<< HEAD
    switch (opcode)
=======
    if(isfloat && operands[1]->getType()->isInt() && operands[1]->getSymPtr()->isConstant())
    {
        s2 += ".0";
    }
    switch(opcode)
>>>>>>> fb5c90b429e0844b7b29767f93558e20029563fb
    {
    case ADD:
        if (isfloat)
            op = "fadd";
        else
            op = "add";
        break;
    case SUB:
        if (isfloat)
            op = "fsub";
        else
            op = "sub";
        break;
    case NOT:
        fprintf(yyout, "  %s = xor %s %s, true\n", s1.c_str(), operands[1]->getType()->toStr().c_str(), s2.c_str());
        return;
    }
<<<<<<< HEAD
    fprintf(yyout, "  %s = %s %s 0, %s\n", s1.c_str(), op.c_str(), operands[1]->getType()->toStr().c_str(), s2.c_str());
=======
    if(isfloat)
        fprintf(yyout, "  %s = %s %s 0.0, %s\n", s1.c_str(), op.c_str(), operands[1]->getType()->toStr().c_str(), s2.c_str());
    else
        fprintf(yyout, "  %s = %s %s 0, %s\n", s1.c_str(), op.c_str(), operands[1]->getType()->toStr().c_str(), s2.c_str());

>>>>>>> fb5c90b429e0844b7b29767f93558e20029563fb
}

CmpInstruction::CmpInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2, BasicBlock *insert_bb) : Instruction(CMP, insert_bb)
{
    this->opcode = opcode;
    operands.push_back(dst);
    operands.push_back(src1);
    operands.push_back(src2);
    dst->setDef(this);
    src1->addUse(this);
    src2->addUse(this);
}

CmpInstruction::~CmpInstruction()
{
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
    operands[2]->removeUse(this);
}

void CmpInstruction::output() const
{
    std::string s1, s2, s3, op, type, ident;
    s1 = operands[0]->toStr();
    s2 = operands[1]->toStr();
    s3 = operands[2]->toStr();
    type = operands[1]->getType()->toStr();
    if(operands[1]->getType()->isFloat())
    {
        ident = "fcmp";
        switch (opcode)
        {
        case E:
            op = "oeq";
            break;
        case NE:
            op = "one";
            break;
        case L:
            op = "olt";
            break;
        case LE:
            op = "ole";
            break;
        case G:
            op = "ogt";
            break;
        case GE:
            op = "oge";
            break;
        default:
            op = "";
            break;
        }
        if(operands[1]->getType()->isInt() && operands[1]->getSymPtr()->isConstant())
        {
            s2 += ".0";
        }
        if(operands[2]->getType()->isInt() && operands[2]->getSymPtr()->isConstant())
        {
            s3 += ".0";
        }
    }
    else if(operands[1]->getType()->isInt())
    {
        ident = "icmp";
        switch (opcode)
        {
        case E:
            op = "eq";
            break;
        case NE:
            op = "ne";
            break;
        case L:
            op = "slt";
            break;
        case LE:
            op = "sle";
            break;
        case G:
            op = "sgt";
            break;
        case GE:
            op = "sge";
            break;
        default:
            op = "";
            break;
        }
    }

    fprintf(yyout, "  %s = %s %s %s %s, %s\n", s1.c_str(), ident.c_str(), op.c_str(), type.c_str(), s2.c_str(), s3.c_str());
}

UncondBrInstruction::UncondBrInstruction(BasicBlock *to, BasicBlock *insert_bb) : Instruction(UNCOND, insert_bb)
{
    branch = to;
}

void UncondBrInstruction::output() const
{
    fprintf(yyout, "  br label %%B%d\n", branch->getNo());
}

void UncondBrInstruction::setBranch(BasicBlock *bb)
{
    branch = bb;
}

BasicBlock *UncondBrInstruction::getBranch()
{
    return branch;
}

CondBrInstruction::CondBrInstruction(BasicBlock *true_branch, BasicBlock *false_branch, Operand *cond, BasicBlock *insert_bb) : Instruction(COND, insert_bb)
{
    this->true_branch = true_branch;
    this->false_branch = false_branch;
    cond->addUse(this);
    operands.push_back(cond);
}

CondBrInstruction::~CondBrInstruction()
{
    operands[0]->removeUse(this);
}

void CondBrInstruction::output() const
{
    std::string cond, type;
    cond = operands[0]->toStr();
    type = operands[0]->getType()->toStr();
    int true_label = true_branch->getNo();
    int false_label = false_branch->getNo();
    fprintf(yyout, "  br %s %s, label %%B%d, label %%B%d\n", type.c_str(), cond.c_str(), true_label, false_label);
}

void CondBrInstruction::setFalseBranch(BasicBlock *bb)
{
    false_branch = bb;
}

BasicBlock *CondBrInstruction::getFalseBranch()
{
    return false_branch;
}

void CondBrInstruction::setTrueBranch(BasicBlock *bb)
{
    true_branch = bb;
}

BasicBlock *CondBrInstruction::getTrueBranch()
{
    return true_branch;
}

RetInstruction::RetInstruction(Operand *src, BasicBlock *insert_bb) : Instruction(RET, insert_bb)
{
    if (src != nullptr)
    {
        operands.push_back(src);
        src->addUse(this);
    }
}

RetInstruction::~RetInstruction()
{
    if (!operands.empty())
        operands[0]->removeUse(this);
}

void RetInstruction::output() const
{
    if (operands.empty())
    {
        fprintf(yyout, "  ret void\n");
    }
    else
    {
        std::string ret, type;
        ret = operands[0]->toStr();
        type = operands[0]->getType()->toStr();
        fprintf(yyout, "  ret %s %s\n", type.c_str(), ret.c_str());
    }
}

FuncCallInstruction::FuncCallInstruction(SymbolEntry *se, Operand *dst, std::vector<Operand *> params, BasicBlock *insert_bb = nullptr) : Instruction(FUNCCALL, insert_bb), se(se), dst(dst), params(params)
{
    dst->setDef(this);
    for (auto t : params)
    {
        t->addUse(this);
    }
}

FuncCallInstruction::~FuncCallInstruction()
{
    dst->setDef(nullptr);
    if (dst->usersNum() == 0)
        delete dst;
    for (auto t : params)
    {
        t->removeUse(this);
    }
}

void FuncCallInstruction::output() const
{
    std::string dstr, type, paramStr;
    dstr = dst->toStr();
    type = ((FunctionType *)se->getType())->getRetType()->toStr();
    if (params.size() == 0)
    {
        paramStr = "";
    }
    else
    {
        paramStr += params[0]->getType()->toStr() + " " + params[0]->toStr();
        for (unsigned int i = 1; i < params.size(); i++)
        {
            paramStr += ", " + params[i]->getType()->toStr() + " " + params[i]->toStr();
        }
    }
    if (((FunctionType *)se->getType())->getRetType()->isVoid())
    {
        fprintf(yyout, "  call %s %s(%s)\n", type.c_str(), se->toStr().c_str(), paramStr.c_str());
    }
    else
        fprintf(yyout, "  %s = call %s %s(%s)\n", dstr.c_str(), type.c_str(), se->toStr().c_str(), paramStr.c_str());
}

AllocaInstruction::AllocaInstruction(Operand *dst, SymbolEntry *se, BasicBlock *insert_bb) : Instruction(ALLOCA, insert_bb)
{
    operands.push_back(dst);
    dst->setDef(this);
    this->se = se;
}

AllocaInstruction::~AllocaInstruction()
{
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
}

void AllocaInstruction::output() const
{
    std::string dst, type;
    dst = operands[0]->toStr();
    type = se->getType()->toStr();
    fprintf(yyout, "  %s = alloca %s, align 4\n", dst.c_str(), type.c_str());
}

LoadInstruction::LoadInstruction(Operand *dst, Operand *src_addr, BasicBlock *insert_bb) : Instruction(LOAD, insert_bb)
{
    operands.push_back(dst);
    operands.push_back(src_addr);
    dst->setDef(this);
    src_addr->addUse(this);
}

LoadInstruction::~LoadInstruction()
{
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
}

void LoadInstruction::output() const
{
    std::string dst = operands[0]->toStr();
    std::string src = operands[1]->toStr();
    std::string src_type;
    std::string dst_type;
    dst_type = operands[0]->getType()->toStr();
    src_type = operands[1]->getType()->toStr();
    fprintf(yyout, "  %s = load %s, %s %s, align 4\n", dst.c_str(), dst_type.c_str(), src_type.c_str(), src.c_str());
}

StoreInstruction::StoreInstruction(Operand *dst_addr, Operand *src, BasicBlock *insert_bb) : Instruction(STORE, insert_bb)
{
    operands.push_back(dst_addr);
    operands.push_back(src);
    dst_addr->addUse(this);
    src->addUse(this);
}

StoreInstruction::~StoreInstruction()
{
    operands[0]->removeUse(this);
    operands[1]->removeUse(this);
}

void StoreInstruction::output() const
{
    std::string dst = operands[0]->toStr();
    std::string src = operands[1]->toStr();
    std::string dst_type = operands[0]->getType()->toStr();
    std::string src_type = operands[1]->getType()->toStr();
    if(operands[1]->getType()->isFloat() && operands[1]->getType()->isInt() && operands[1]->getSymPtr()->isConstant())
        src += ".0";

    fprintf(yyout, "  store %s %s, %s %s, align 4\n", src_type.c_str(), src.c_str(), dst_type.c_str(), dst.c_str());
}

TypeConverInstruction::TypeConverInstruction(Operand *dst, Operand *src, BasicBlock *insert_bb) : Instruction(TYPECONVER, insert_bb), dst(dst), src(src)
{
    dst->setDef(this);
    src->addUse(this);
}

TypeConverInstruction::~TypeConverInstruction()
{
    dst->setDef(nullptr);
    src->addUse(this);
}

void TypeConverInstruction::output() const
{
    // eg. %7 = sitofp i32 %6 to float
    std::string typeConver;
    if (src->getType() == TypeSystem::boolType && dst->getType()->isInt())
    {
        typeConver = "zext";
    }
    else if (src->getType()->isFloat() && dst->getType()->isInt())
    {
        typeConver = "fptosi";
    }
    else if (src->getType()->isInt() && dst->getType()->isFloat())
    {
        typeConver = "sitofp";
    }
    fprintf(yyout, "  %s = %s %s %s to %s\n", dst->toStr().c_str(), typeConver.c_str(), src->getType()->toStr().c_str(), src->toStr().c_str(), dst->getType()->toStr().c_str());
}

GlobalVarDefInstruction::GlobalVarDefInstruction(Operand *dst, ConstantSymbolEntry *se, BasicBlock *insert_bb) : Instruction(GLOBALVAR, insert_bb), dst(dst)
{
    type = ((PointerType *)dst->getType())->getValueType();
    if (se == nullptr)
    {

        if (type->isInt())
            value.intValue = 0;
        else if (type->isFloat())
            value.floatValue = 0;
    }
    else
    {
        if (se->getType()->isInt())
        {
            if (type->isInt())
                value.intValue = se->getInt();
            else if (type->isFloat())
                value.intValue = se->getFloat();
        }
        else if (se->getType()->isFloat())
        {
            if (type->isInt())
                value.floatValue = se->getInt();
            else if (type->isFloat())
                value.floatValue = se->getFloat();
        }
    }
    dst->setDef(this);
}

GlobalVarDefInstruction::~GlobalVarDefInstruction()
{
    dst->setDef(nullptr);
}

void GlobalVarDefInstruction::output() const
{
<<<<<<< HEAD
    if (type->isInt())
=======
    std::string ident;
    if(dst->isConst())
        ident = "constant";
    else
        ident = "global";
    if(type->isInt())
>>>>>>> fb5c90b429e0844b7b29767f93558e20029563fb
    {
        fprintf(yyout, "%s = %s i32 %d, align 4\n", dst->toStr().c_str(), ident.c_str(), value.intValue);
    }
    else if (type->isFloat())
    {
        fprintf(yyout, "%s = %s float %e, align 4\n", dst->toStr().c_str(), ident.c_str(), value.floatValue);
    }
}

ToBoolInstruction::ToBoolInstruction(Operand *dst, Operand *src, BasicBlock *insert_bb) : Instruction(TOBOOL, insert_bb), dst(dst), src(src)
{
    src->addUse(this);
    dst->setDef(this);
}

ToBoolInstruction::~ToBoolInstruction()
{
    src->removeUse(this);
    dst->setDef(nullptr);
}

void ToBoolInstruction::output() const
{
    std::string s1, s2, s3, op, type;
    s1 = src->toStr();
    s2 = dst->toStr();
    type = src->getType()->toStr();

    fprintf(yyout, "  %s = icmp ne %s %s, 0\n", s2.c_str(), type.c_str(), s1.c_str());
}