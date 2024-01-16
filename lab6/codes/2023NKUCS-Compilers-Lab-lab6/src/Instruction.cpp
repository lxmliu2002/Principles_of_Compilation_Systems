#include "Instruction.h"
#include <iostream>
#include <sstream>
#include "BasicBlock.h"
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

void Instruction::remove()
{
    this->getNext()->setPrev(this->getPrev());
    this->getPrev()->setNext(this->getNext());
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
    {
        delete operands[0];
    }
    int opdcnt = operands.size();
    for (int i = 1; i < opdcnt; i++)
    {
        operands[i]->removeUse(this);
    }
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
    {
        delete operands[0];
    }
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
    switch (opcode)
    {
    case ADD:
        op = "add";
        break;
    case SUB:
        op = "sub";
        break;
    case MUL:
        op = "mul";
        break;
    case DIV:
        op = "sdiv";
        break;
    case MOD:
        op = "srem";
        break;
    default:
        break;
    }
    fprintf(yyout, "  %s = %s %s %s, %s\n", s1.c_str(), op.c_str(), type.c_str(), s2.c_str(), s3.c_str());
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
    {
        delete operands[0];
    }
    operands[1]->removeUse(this);
    operands[2]->removeUse(this);
}

void CmpInstruction::output() const
{
    std::string s1, s2, s3, op, type;
    s1 = operands[0]->toStr();
    s2 = operands[1]->toStr();
    s3 = operands[2]->toStr();
    type = operands[1]->getType()->toStr();
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

    fprintf(yyout, "  %s = icmp %s %s %s, %s\n", s1.c_str(), op.c_str(), type.c_str(), s2.c_str(), s3.c_str());
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
    {
        delete operands[0];
    }
}

void AllocaInstruction::output() const
{
    std::string dst, type;
    dst = operands[0]->toStr();
    if (se->getType()->isInt() || se->getType()->isFloat())
    {
        type = se->getType()->toStr();
        fprintf(yyout, "  %s = alloca %s, align 4\n", dst.c_str(), type.c_str());
    }
    else if (se->getType()->isArray())
    {
        type = se->getType()->toStr();
        fprintf(yyout, "  %s = alloca %s, align 4\n", dst.c_str(), type.c_str());
    }
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
    {
        delete operands[0];
    }
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

StoreInstruction::StoreInstruction(Operand *dst_addr, Operand *src, BasicBlock *insert_bb)
    : Instruction(STORE, insert_bb)
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

    fprintf(yyout, "  store %s %s, %s %s, align 4\n", src_type.c_str(), src.c_str(), dst_type.c_str(), dst.c_str());
}

MachineOperand *Instruction::genMachineOperand(Operand *ope)
{
    auto se = ope->getEntry();
    MachineOperand *mope = nullptr;
    if (se->isConstant())
    {
        if (se->getType()->isFloat())
        {
            mope = new MachineOperand(MachineOperand::IMM, (float)dynamic_cast<ConstantSymbolEntry *>(se)->getValue());
        }
        else
        {
            mope = new MachineOperand(MachineOperand::IMM, (int)dynamic_cast<ConstantSymbolEntry *>(se)->getValue());
        }
    }
    else if (se->isTemporary())
        mope = new MachineOperand(MachineOperand::VREG, dynamic_cast<TemporarySymbolEntry *>(se)->getLabel());
    else if (se->isVariable())
    {
        auto id_se = dynamic_cast<IdentifierSymbolEntry *>(se);
        if (id_se->isGlobal())
        {
            mope = new MachineOperand(id_se->toStr().c_str());
        }
        else if (id_se->isParam())
        {
            if (id_se->getParamNo() < 4)
            {
                mope = new MachineOperand(MachineOperand::REG, id_se->getParamNo());
            }
            else
            {
                mope = new MachineOperand(MachineOperand::REG, 3);
            }
        }
        else
        {
            exit(0);
        }
    }
    return mope;
}

MachineOperand *Instruction::genMachineReg(int reg)
{
    return new MachineOperand(MachineOperand::REG, reg);
}

MachineOperand *Instruction::genMachineVReg()
{
    return new MachineOperand(MachineOperand::VREG, SymbolTable::getLabel());
}

MachineOperand *Instruction::genMachineImm(int val)
{
    return new MachineOperand(MachineOperand::IMM, val);
}

MachineOperand *Instruction::genMachineLabel(int block_no)
{
    std::ostringstream buf;
    buf << ".L" << block_no;
    std::string label = buf.str();
    return new MachineOperand(label);
}

void AllocaInstruction::genMachineCode(AsmBuilder *builder)
{
    auto cur_func = builder->getFunction();
    int size = se->getType()->getSize() / 8;
    if (size < 0)
    {
        size = 4;
    }
    int offset = cur_func->AllocSpace(size);
    dynamic_cast<TemporarySymbolEntry *>(operands[0]->getEntry())->setOffset(-offset);
}

void LoadInstruction::genMachineCode(AsmBuilder *builder)
{
    auto cur_block = builder->getBlock();
    MachineInstruction *cur_inst = nullptr;
    if (operands[1]->getEntry()->isVariable() && dynamic_cast<IdentifierSymbolEntry *>(operands[1]->getEntry())->isGlobal())//全局变量
    {
        auto dst = genMachineOperand(operands[0]);
        auto internal_reg1 = genMachineVReg();
        auto internal_reg2 = new MachineOperand(*internal_reg1);
        auto src = genMachineOperand(operands[1]);
        cur_inst = new LoadMInstruction(cur_block, internal_reg1, src);
        cur_block->InsertInst(cur_inst);
        cur_inst = new LoadMInstruction(cur_block, dst, internal_reg2);
        cur_block->InsertInst(cur_inst);
    }
    else if (operands[1]->getEntry()->isTemporary() && operands[1]->getDef() && operands[1]->getDef()->isAlloc())//从栈上加载的临时变量
    {
        auto dst = genMachineOperand(operands[0]);
        auto src1 = genMachineReg(11);
        int off = dynamic_cast<TemporarySymbolEntry *>(operands[1]->getEntry())->getOffset();
        auto src2 = genMachineImm(off);
        cur_inst = new LoadMInstruction(cur_block, dst, src1, src2);
        cur_block->InsertInst(cur_inst);
    }
    else
    {
        auto dst = genMachineOperand(operands[0]);
        auto src = genMachineOperand(operands[1]);
        cur_inst = new LoadMInstruction(cur_block, dst, src);
        cur_block->InsertInst(cur_inst);
    }
}

void StoreInstruction::genMachineCode(AsmBuilder *builder)
{
    auto cur_block = builder->getBlock();
    MachineInstruction *cur_inst = nullptr;
    auto dst = genMachineOperand(operands[0]);
    auto src = genMachineOperand(operands[1]);
    if (operands[1]->getEntry()->isConstant())// 如果源操作数是常数 先加载到一个虚拟寄存器
    {
        auto dst1 = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block, dst1, src);
        cur_block->InsertInst(cur_inst);
        src = new MachineOperand(*dst1);
    }
    if (operands[0]->getEntry()->isTemporary() && operands[0]->getDef() && operands[0]->getDef()->isAlloc())
    {
        auto src1 = genMachineReg(11);
        int off = dynamic_cast<TemporarySymbolEntry *>(operands[0]->getEntry())->getOffset();
        auto src2 = genMachineImm(off);
        cur_inst = new StoreMInstruction(cur_block, src, src1, src2);
        cur_block->InsertInst(cur_inst);
    }
    else if (operands[0]->getEntry()->isVariable() && dynamic_cast<IdentifierSymbolEntry *>(operands[0]->getEntry())->isGlobal())
    {
        auto internal_reg1 = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block, internal_reg1, dst);
        cur_block->InsertInst(cur_inst);
        cur_inst = new StoreMInstruction(cur_block, src, internal_reg1);
        cur_block->InsertInst(cur_inst);
    }
    else if (operands[0]->getType()->isPtr())
    {
        cur_inst = new StoreMInstruction(cur_block, src, dst);
        cur_block->InsertInst(cur_inst);
    }
}

void BinaryInstruction::genMachineCode(AsmBuilder *builder)
{
    auto cur_block = builder->getBlock();
    auto dst = genMachineOperand(operands[0]);
    auto src1 = genMachineOperand(operands[1]);
    auto src2 = genMachineOperand(operands[2]);

    if (operands[0]->getType()->isFloat())
    {
        MachineInstruction *cur_inst;

        auto operand1 = genMachineReg(0);
        auto operand2 = genMachineReg(1);

        if (src1->isImm())
        {
            cur_inst = new LoadMInstruction(cur_block, operand1, src1);
        }
        else
        {
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, operand1, src1);
        }
        cur_block->InsertInst(cur_inst);

        if (src2->isImm())
        {
            cur_inst = new LoadMInstruction(cur_block, operand2, src2);
        }
        else
        {
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, operand2, src2);
        }
        cur_block->InsertInst(cur_inst);

        std::string abi_str;

        switch (opcode)
        {
        case ADD:
            abi_str = "@__aeabi_fadd";
            break;
        case SUB:
            abi_str = "@__aeabi_fsub";
            break;
        case MUL:
            abi_str = "@__aeabi_fmul";
            break;
        case DIV:
            abi_str = "@__aeabi_fdiv";
            break;
        default:
            // error
            break;
        }

        auto label = new MachineOperand(abi_str);
        cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::BL, label);
        cur_block->InsertInst(cur_inst);

        auto r0 = new MachineOperand(MachineOperand::REG, 0);
        cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, r0);
        cur_block->InsertInst(cur_inst);

        return;
    }

    MachineInstruction *cur_inst = nullptr;
    if (src1->isImm())
    {
        auto internal_reg = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block, internal_reg, src1);
        cur_block->InsertInst(cur_inst);
        src1 = new MachineOperand(*internal_reg);
    }
    if (src2->isImm())
    {
        if (opcode >= BinaryInstruction::MUL)
        {
            auto internal_reg = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block, internal_reg, src2);
            cur_block->InsertInst(cur_inst);
            src2 = new MachineOperand(*internal_reg);
        }
    }
    switch (opcode)
    {
    case ADD:
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, dst, src1, src2);
        break;
    case SUB:
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::SUB, dst, src1, src2);
        break;
    case AND:
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::AND, dst, src1, src2);
        break;
    case OR:
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::OR, dst, src1, src2);
        break;
    case MUL:
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::MUL, dst, src1, src2);
        break;
    case DIV:
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::DIV, dst, src1, src2);
        break;
    case MOD:
    {
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::DIV, dst, src1, src2);
        MachineOperand *dst1 = new MachineOperand(*dst);
        src1 = new MachineOperand(*src1);
        src2 = new MachineOperand(*src2);
        cur_block->InsertInst(cur_inst);
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::MUL, dst1, dst, src2);
        cur_block->InsertInst(cur_inst);
        dst = new MachineOperand(*dst1);
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::SUB, dst, src1, dst1);
        break;
    }
    default:
        break;
    }
    cur_block->InsertInst(cur_inst);
}

void CmpInstruction::genMachineCode(AsmBuilder *builder)
{
    auto cur_block = builder->getBlock();
    auto src1 = genMachineOperand(operands[1]);
    auto src2 = genMachineOperand(operands[2]);

    if (operands[1]->getType()->isFloat())
    {
        MachineInstruction *cur_inst;

        auto operand1 = genMachineReg(0);
        auto operand2 = genMachineReg(1);

        if (src1->isImm())
        {
            cur_inst = new LoadMInstruction(cur_block, operand1, src1);
        }
        else
        {
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, operand1, src1);
        }
        cur_block->InsertInst(cur_inst);

        if (src2->isImm())
        {
            cur_inst = new LoadMInstruction(cur_block, operand2, src2);
        }
        else
        {
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, operand2, src2);
        }
        cur_block->InsertInst(cur_inst);

        std::string abi_str;

        int cmp_imm = 1;

        switch (opcode)
        {
        case CmpInstruction::L:
            abi_str = "@__aeabi_fcmplt";
            break;
        case CmpInstruction::LE:
            abi_str = "@__aeabi_fcmple";
            break;
        case CmpInstruction::G:
            abi_str = "@__aeabi_fcmpgt";
            break;
        case CmpInstruction::GE:
            abi_str = "@__aeabi_fcmpge";
            break;
        case CmpInstruction::E:
            abi_str = "@__aeabi_fcmpeq";
            break;
        case CmpInstruction::NE:
            abi_str = "@__aeabi_fcmpeq";
            cmp_imm = 0;
            break;
        default:
            // error
            break;
        }

        auto label = new MachineOperand(abi_str);
        cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::BL, label);
        cur_block->InsertInst(cur_inst);

        auto r0 = new MachineOperand(MachineOperand::REG, 0);
        auto dst = genMachineOperand(operands[0]);

        auto bool_operand = genMachineImm(cmp_imm);
        cur_inst = new CmpMInstruction(cur_block, r0, bool_operand, CmpMInstruction::EQ);
        cur_block->InsertInst(cur_inst);

        cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, genMachineImm(0), CmpInstruction::NE);
        cur_block->InsertInst(cur_inst);
        cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, genMachineImm(1), CmpInstruction::E);
        cur_block->InsertInst(cur_inst);

        return;
    }

    MachineInstruction *cur_inst = nullptr;
    if (src1->isImm())
    {
        auto internal_reg = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block, internal_reg, src1);
        cur_block->InsertInst(cur_inst);
        src1 = new MachineOperand(*internal_reg);
    }
    if (src2->isImm())
    {
        auto internal_reg = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block, internal_reg, src2);
        cur_block->InsertInst(cur_inst);
        src2 = new MachineOperand(*internal_reg);
    }
    cur_inst = new CmpMInstruction(cur_block, src1, src2, opcode);
    cur_block->InsertInst(cur_inst);
    if (opcode >= CmpInstruction::L && opcode <= CmpInstruction::GE)
    {
        auto dst = genMachineOperand(operands[0]);
        auto trueOperand = genMachineImm(1);
        auto falseOperand = genMachineImm(0);
        cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, trueOperand, opcode);
        cur_block->InsertInst(cur_inst);
        cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, falseOperand, 7 - opcode);
        cur_block->InsertInst(cur_inst);
    }
}

void UncondBrInstruction::genMachineCode(AsmBuilder *builder)
{
    auto cur_block = builder->getBlock();
    std::stringstream s;
    s << ".L" << branch->getNo();
    MachineOperand *dst = new MachineOperand(s.str());
    auto cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::B, dst);
    cur_block->InsertInst(cur_inst);
}

void CondBrInstruction::genMachineCode(AsmBuilder *builder)
{
    auto cur_block = builder->getBlock();
    std::stringstream s;
    s << ".L" << true_branch->getNo();
    MachineOperand *dst = new MachineOperand(s.str());
    auto cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::B, dst, cur_block->getCmpCond());
    cur_block->InsertInst(cur_inst);
    s.str("");
    s << ".L" << false_branch->getNo();
    dst = new MachineOperand(s.str());
    cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::B, dst);
    cur_block->InsertInst(cur_inst);
}

void RetInstruction::genMachineCode(AsmBuilder *builder)
{
    auto cur_block = builder->getBlock();
    if (!operands.empty())
    {
        auto dst = new MachineOperand(MachineOperand::REG, 0);
        auto src = genMachineOperand(operands[0]);
        auto cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, src);
        cur_block->InsertInst(cur_inst);
    }
    auto cur_func = builder->getFunction();
    auto sp = new MachineOperand(MachineOperand::REG, 13);
    auto size = new MachineOperand(MachineOperand::IMM, cur_func->AllocSpace(0));
    auto cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, sp, sp, size);
    cur_block->InsertInst(cur_inst);
    auto lr = new MachineOperand(MachineOperand::REG, 14);
    auto cur_inst2 = new BranchMInstruction(cur_block, BranchMInstruction::BX, lr);
    cur_block->InsertInst(cur_inst2);
}

CallInstruction::CallInstruction(Operand *dst, SymbolEntry *func, std::vector<Operand *> params, BasicBlock *insert_bb) : Instruction(CALL, insert_bb), func(func), dst(dst)
{
    operands.push_back(dst);
    if (dst)
    {
        dst->setDef(this);
    }
    for (auto param : params)
    {
        operands.push_back(param);
        param->addUse(this);
    }
}

void CallInstruction::output() const
{
    fprintf(yyout, "  ");
    if (operands[0])
    {
        fprintf(yyout, "%s = ", operands[0]->toStr().c_str());
    }
    FunctionType *type = (FunctionType *)(func->getType());
    fprintf(yyout, "call %s %s(", type->getRetType()->toStr().c_str(), func->toStr().c_str());
    for (long unsigned int i = 1; i < operands.size(); i++)
    {
        if (i != 1)
        {
            fprintf(yyout, ", ");
        }
        fprintf(yyout, "%s %s", operands[i]->getType()->toStr().c_str(), operands[i]->toStr().c_str());
    }
    fprintf(yyout, ")\n");
}

CallInstruction::~CallInstruction()
{
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
    {
        delete operands[0];
    }
    for (long unsigned int i = 1; i < operands.size(); i++)
    {
        operands[i]->removeUse(this);
    }
}

ZextInstruction::ZextInstruction(Operand *dst, Operand *src, BasicBlock *insert_bb) : Instruction(ZEXT, insert_bb)
{
    operands.push_back(dst);
    operands.push_back(src);
    dst->setDef(this);
    src->addUse(this);
}

void ZextInstruction::output() const
{
    Operand *dst = operands[0];
    Operand *src = operands[1];
    fprintf(yyout, "  %s = zext %s %s to i32\n", dst->toStr().c_str(), src->getType()->toStr().c_str(), src->toStr().c_str());
}

ZextInstruction::~ZextInstruction()
{
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
    {
        delete operands[0];
    }
    operands[1]->removeUse(this);
}

XorInstruction::XorInstruction(Operand *dst, Operand *src, BasicBlock *insert_bb) : Instruction(XOR, insert_bb)
{
    operands.push_back(dst);
    operands.push_back(src);
    dst->setDef(this);
    src->addUse(this);
}

void XorInstruction::output() const
{
    Operand *dst = operands[0];
    Operand *src = operands[1];
    fprintf(yyout, "  %s = xor %s %s, true\n", dst->toStr().c_str(), src->getType()->toStr().c_str(), src->toStr().c_str());
}

XorInstruction::~XorInstruction()
{
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
    {
        delete operands[0];
    }
    operands[1]->removeUse(this);
}

GepInstruction::GepInstruction(Operand *dst, Operand *arr, Operand *idx, BasicBlock *insert_bb, bool paramFirst) : Instruction(GEP, insert_bb), paramFirst(paramFirst)
{
    operands.push_back(dst);
    operands.push_back(arr);
    operands.push_back(idx);
    dst->setDef(this);
    arr->addUse(this);
    idx->addUse(this);
    first = false;
    init = nullptr;
    last = false;
}

void GepInstruction::output() const
{
    Operand *dst = operands[0];
    Operand *arr = operands[1];
    Operand *idx = operands[2];
    std::string arrType = arr->getType()->toStr();
    if (paramFirst)
    {
        fprintf(yyout, "  %s = getelementptr inbounds %s, %s %s, i32 %s\n", dst->toStr().c_str(), arrType.substr(0, arrType.size() - 1).c_str(), arrType.c_str(), arr->toStr().c_str(), idx->toStr().c_str());
    }
    else
    {
        fprintf(yyout, "  %s = getelementptr inbounds %s, %s %s, i32 0, i32 %s\n", dst->toStr().c_str(), arrType.substr(0, arrType.size() - 1).c_str(), arrType.c_str(), arr->toStr().c_str(), idx->toStr().c_str());
    }
}

GepInstruction::~GepInstruction()
{
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
    {
        delete operands[0];
    }
    operands[1]->removeUse(this);
    operands[2]->removeUse(this);
}

FptosiInstruction::FptosiInstruction(Operand *dst, Operand *src, BasicBlock *insert_bb) : Instruction(FPTOSI, insert_bb), dst(dst), src(src)
{
    operands.push_back(dst);
    operands.push_back(src);
    dst->setDef(this);
    src->addUse(this);
}

void FptosiInstruction::output() const
{
    Operand *dst = operands[0];
    Operand *src = operands[1];
    fprintf(yyout, "  %s = fptosi %s %s to %s\n", dst->toStr().c_str(), src->getType()->toStr().c_str(), src->toStr().c_str(), dst->getType()->toStr().c_str());
}

FptosiInstruction::~FptosiInstruction()
{
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
    {
        delete operands[0];
    }
    operands[1]->removeUse(this);
}

SitofpInstruction::SitofpInstruction(Operand *dst, Operand *src, BasicBlock *insert_bb) : Instruction(FPTOSI, insert_bb), dst(dst), src(src)
{
    operands.push_back(dst);
    operands.push_back(src);
    dst->setDef(this);
    src->addUse(this);
}

void SitofpInstruction::output() const
{
    Operand *dst = operands[0];
    Operand *src = operands[1];
    fprintf(yyout, "  %s = sitofp %s %s to %s\n", dst->toStr().c_str(), src->getType()->toStr().c_str(), src->toStr().c_str(), dst->getType()->toStr().c_str());
}

SitofpInstruction::~SitofpInstruction()
{
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
    {
        delete operands[0];
    }
    operands[1]->removeUse(this);
}

void CallInstruction::genMachineCode(AsmBuilder *builder)
{
    auto cur_block = builder->getBlock();
    MachineOperand *operand; //, *num;
    MachineInstruction *cur_inst;
    // auto fp = new MachineOperand(MachineOperand::REG, 11);
    int idx = 0;
    for (auto it = operands.begin(); it != operands.end(); it++, idx++)
    {
        if (idx == 0)
        {
            continue;
        }
        if (idx == 5)
        {
            break;
        }
        operand = genMachineReg(idx - 1);
        auto src = genMachineOperand(operands[idx]);
        cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, operand, src);
        cur_block->InsertInst(cur_inst);
    }
    for (int i = operands.size() - 1; i > 4; i--)
    {
        operand = genMachineOperand(operands[i]);
        if (operand->isImm())
        {
            auto dst = genMachineVReg();
            if (operand->getVal() < 256)
            {
                cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, operand);
            }
            else
            {
                cur_inst = new LoadMInstruction(cur_block, dst, operand);
            }
            cur_block->InsertInst(cur_inst);
            operand = dst;
        }
        std::vector<MachineOperand *> vec;
        cur_inst = new StackMInstrcuton(cur_block, StackMInstrcuton::PUSH, vec, operand);
        cur_block->InsertInst(cur_inst);
    }
    auto label = new MachineOperand(func->toStr().c_str());
    cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::BL, label);
    cur_block->InsertInst(cur_inst);
    if (operands.size() > 5)
    {
        auto off = genMachineImm((operands.size() - 5) * 4);
        auto sp = new MachineOperand(MachineOperand::REG, 13);
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, sp, sp, off);
        cur_block->InsertInst(cur_inst);
    }
    if (dst)
    {
        operand = genMachineOperand(dst);
        auto r0 = new MachineOperand(MachineOperand::REG, 0);
        cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, operand, r0);
        cur_block->InsertInst(cur_inst);
    }
}

void ZextInstruction::genMachineCode(AsmBuilder *builder)
{
    auto cur_block = builder->getBlock();
    auto dst = genMachineOperand(operands[0]);
    auto src = genMachineOperand(operands[1]);
    auto cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, src);
    cur_block->InsertInst(cur_inst);
}

void XorInstruction::genMachineCode(AsmBuilder *builder)
{
    auto cur_block = builder->getBlock();
    auto dst = genMachineOperand(operands[0]);
    auto trueOperand = genMachineImm(1);
    auto falseOperand = genMachineImm(0);
    auto cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, trueOperand, MachineInstruction::EQ);
    cur_block->InsertInst(cur_inst);
    cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, falseOperand, MachineInstruction::NE);
    cur_block->InsertInst(cur_inst);
}

void GepInstruction::genMachineCode(AsmBuilder *builder)
{
    auto cur_block = builder->getBlock();
    MachineInstruction *cur_inst;
    auto dst = genMachineOperand(operands[0]);//目的寄存器
    auto idx = genMachineOperand(operands[2]);//索引
    if (init)
    {
        if (last)
        {
            auto base = genMachineOperand(init);
            cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, dst, base, genMachineImm(4));
            cur_block->InsertInst(cur_inst);
        }
        return;
    }
    MachineOperand *base = nullptr;
    int size;
    auto idx1 = genMachineVReg();// 加载索引后的值
    if (idx->isImm())
    {
        cur_inst = new LoadMInstruction(cur_block, idx1, idx);
        idx = new MachineOperand(*idx1);
        cur_block->InsertInst(cur_inst);
    }
    if (paramFirst)
    {
        size = ((PointerType *)(operands[1]->getType()))->getType()->getSize() / 8;
    }
    else
    {
        if (first)
        {
            base = genMachineVReg();
            if (operands[1]->getEntry()->isVariable() && ((IdentifierSymbolEntry *)(operands[1]->getEntry()))->isGlobal())
            {
                auto src = genMachineOperand(operands[1]);
                cur_inst = new LoadMInstruction(cur_block, base, src);
            }
            else
            {
                int offset = ((TemporarySymbolEntry *)(operands[1]->getEntry()))->getOffset();
                cur_inst = new LoadMInstruction(cur_block, base, genMachineImm(offset));
            }
            cur_block->InsertInst(cur_inst);
        }
        ArrayType *type = (ArrayType *)(((PointerType *)(operands[1]->getType()))->getType());
        Type *elementType = type->getElementType();
        size = elementType->getSize() / 8;
    }
    // 存储元素大小
    auto size1 = genMachineVReg();
    cur_inst = new LoadMInstruction(cur_block, size1, genMachineImm(size));
    cur_block->InsertInst(cur_inst);
    // 存储偏移量
    auto off = genMachineVReg();
    cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::MUL, off, idx, size1);
    off = new MachineOperand(*off);
    cur_block->InsertInst(cur_inst);
    if (paramFirst || !first)
    {
        auto arr = genMachineOperand(operands[1]);
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, dst, arr, off);
        cur_block->InsertInst(cur_inst);
    }
    else
    {
        auto addr = genMachineVReg();
        auto base1 = new MachineOperand(*base);
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, addr, base1, off);
        cur_block->InsertInst(cur_inst);
        addr = new MachineOperand(*addr);
        if (operands[1]->getEntry()->isVariable() && ((IdentifierSymbolEntry *)(operands[1]->getEntry()))->isGlobal())
        {
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, addr);
        }
        else
        {
            auto fp = genMachineReg(11);
            cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, dst, fp, addr);
        }
        cur_block->InsertInst(cur_inst);
    }
}

void FptosiInstruction::genMachineCode(AsmBuilder *builder)//浮点->整型
{
    MachineInstruction *cur_inst;
    auto cur_block = builder->getBlock();

    auto src_operand = genMachineOperand(src);
    auto operand = genMachineReg(0);

    if (src_operand->isImm())
    {
        cur_inst = new LoadMInstruction(cur_block, operand, src_operand);
    }
    else
    {
        cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, operand, src_operand);
    }
    cur_block->InsertInst(cur_inst);

    auto label = new MachineOperand("@__aeabi_f2iz");//库函数
    cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::BL, label);
    cur_block->InsertInst(cur_inst);

    // 结果写回目的寄存器
    operand = genMachineOperand(dst);
    auto r0 = new MachineOperand(MachineOperand::REG, 0);
    cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, operand, r0);
    cur_block->InsertInst(cur_inst);
}

void SitofpInstruction::genMachineCode(AsmBuilder *builder)//整型->浮点
{
    MachineInstruction *cur_inst;
    auto cur_block = builder->getBlock();

    auto src_operand = genMachineOperand(src);
    auto operand = genMachineReg(0);

    if (src_operand->isImm())
    {
        cur_inst = new LoadMInstruction(cur_block, operand, src_operand);
    }
    else
    {
        cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, operand, src_operand);
    }
    cur_block->InsertInst(cur_inst);

    auto label = new MachineOperand("@__aeabi_i2f");
    cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::BL, label);
    cur_block->InsertInst(cur_inst);

    operand = genMachineOperand(dst);
    auto r0 = new MachineOperand(MachineOperand::REG, 0);
    cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, operand, r0);
    cur_block->InsertInst(cur_inst);
}