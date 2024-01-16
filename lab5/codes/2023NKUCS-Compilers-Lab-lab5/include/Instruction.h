#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include "Operand.h"
#include <vector>
#include <map>

class BasicBlock;

class Instruction
{
public:
    Instruction(unsigned instType, BasicBlock *insert_bb = nullptr);
    virtual ~Instruction();
    BasicBlock *getParent();
    bool isUncond() const { return instType == UNCOND; };
    bool isCond() const { return instType == COND; };
    bool isRet() const { return instType == RET; };
    void setParent(BasicBlock *);
    void setNext(Instruction *);
    void setPrev(Instruction *);
    Instruction *getNext();
    Instruction *getPrev();
    virtual void output() const = 0;

protected:
    unsigned instType;
    unsigned opcode;
    Instruction *prev;
    Instruction *next;
    BasicBlock *parent;
    std::vector<Operand *> operands;
    enum
    {
        BINARY,
        UNARY,
        COND,
        UNCOND,
        RET,
        LOAD,
        STORE,
        CMP,
        ALLOCA,
        FUNCCALL,
        TYPECONVER,
        GLOBALVAR,
        TOBOOL
    };
};

// meaningless instruction, used as the head node of the instruction list.
class DummyInstruction : public Instruction
{
public:
    DummyInstruction() : Instruction(-1, nullptr){};
    void output() const {};
};

class AllocaInstruction : public Instruction
{
public:
    AllocaInstruction(Operand *dst, SymbolEntry *se, BasicBlock *insert_bb = nullptr);
    ~AllocaInstruction();
    void output() const;

private:
    SymbolEntry *se;
};

class LoadInstruction : public Instruction
{
public:
    LoadInstruction(Operand *dst, Operand *src_addr, BasicBlock *insert_bb = nullptr);
    ~LoadInstruction();
    void output() const;
};

class StoreInstruction : public Instruction
{
public:
    StoreInstruction(Operand *dst_addr, Operand *src, BasicBlock *insert_bb = nullptr);
    ~StoreInstruction();
    void output() const;
};

class BinaryInstruction : public Instruction
{
public:
    BinaryInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2, BasicBlock *insert_bb = nullptr);
    ~BinaryInstruction();
    void output() const;
    enum
    {
        SUB,
        ADD,
        MUL,
        DIV,
        MOD,
        AND,
        OR
    };
};

class UnaryInstruction : public Instruction
{
public:
    UnaryInstruction(unsigned opcode, Operand *dst, Operand *src, BasicBlock *insert_bb = nullptr);
    ~UnaryInstruction();
    void output() const;
    enum
    {
        ADD,
        SUB,
        NOT
    };
};

class CmpInstruction : public Instruction
{
public:
    CmpInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2, BasicBlock *insert_bb = nullptr);
    ~CmpInstruction();
    void output() const;
    enum
    {
        E,
        NE,
        L,
        GE,
        G,
        LE
    };
};

// unconditional branch
class UncondBrInstruction : public Instruction
{
public:
    UncondBrInstruction(BasicBlock *, BasicBlock *insert_bb = nullptr);
    void output() const;
    void setBranch(BasicBlock *);
    BasicBlock *getBranch();

protected:
    BasicBlock *branch;
};

// conditional branch
class CondBrInstruction : public Instruction
{
public:
    CondBrInstruction(BasicBlock *, BasicBlock *, Operand *, BasicBlock *insert_bb = nullptr);
    ~CondBrInstruction();
    void output() const;
    void setTrueBranch(BasicBlock *);
    BasicBlock *getTrueBranch();
    void setFalseBranch(BasicBlock *);
    BasicBlock *getFalseBranch();

protected:
    BasicBlock *true_branch;
    BasicBlock *false_branch;
};

class RetInstruction : public Instruction
{
public:
    RetInstruction(Operand *src, BasicBlock *insert_bb = nullptr);
    ~RetInstruction();
    void output() const;
};

class FuncCallInstruction : public Instruction
{
public:
    FuncCallInstruction(SymbolEntry *se, Operand *dst, std::vector<Operand *> params, BasicBlock *insert_bb);
    ~FuncCallInstruction();
    void output() const;

private:
    SymbolEntry *se;
    Operand *dst;
    std::vector<Operand *> params;
};

class TypeConverInstruction : public Instruction
{
public:
    TypeConverInstruction(Operand *dst, Operand *src, BasicBlock *insert_bb = nullptr);
    ~TypeConverInstruction();
    void output() const;

private:
    Operand *dst;
    Operand *src;
};

class GlobalVarDefInstruction : public Instruction
{
public:
    GlobalVarDefInstruction(Operand *dst, ConstantSymbolEntry *se, BasicBlock *insert_bb = nullptr);
    ~GlobalVarDefInstruction();
    void output() const;

private:
    union
    {
        int intValue;
        float floatValue;
    } value;
    Operand *dst;
    Type *type;
};

// class FunctionDeclInstruction : public Instruction
// {
// public:
//     FunctionDeclInstruction(Operand* dst, ConstantSymbolEntry* se, BasicBlock* insert_bb = nullptr);
//     ~FunctionDeclInstruction();
//     void output() const;
// }

class ToBoolInstruction : public Instruction
{
public:
    ToBoolInstruction(Operand *dst, Operand *src, BasicBlock *insert_bb = nullptr);
    ~ToBoolInstruction();
    void output() const;

private:
    Operand *dst;
    Operand *src;
};

#endif