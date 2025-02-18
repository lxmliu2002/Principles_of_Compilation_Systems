#include "BasicBlock.h"
#include <algorithm>
#include "Function.h"
#include <vector>

using namespace std;
extern FILE *yyout;

void BasicBlock::insertFront(Instruction *inst)
{
    insertBefore(inst, head->getNext());
}

void BasicBlock::insertBack(Instruction *inst)
{
    insertBefore(inst, head);
}

void BasicBlock::insertBefore(Instruction *dst, Instruction *src)
{
    src->getPrev()->setNext(dst);
    dst->setPrev(src->getPrev());
    dst->setNext(src);
    src->setPrev(dst);
    dst->setParent(this);
}

void BasicBlock::remove(Instruction *inst)
{
    inst->getPrev()->setNext(inst->getNext());
    inst->getNext()->setPrev(inst->getPrev());
}

void BasicBlock::output() const
{
    fprintf(yyout, "B%d:", no);

    if (!pred.empty())
    {
        fprintf(yyout, "%*c; preds = %%B%d", 32, '\t', pred[0]->getNo());
        for (auto i = pred.begin() + 1; i != pred.end(); i++)
        {
            fprintf(yyout, ", %%B%d", (*i)->getNo());
        }
    }
    fprintf(yyout, "\n");
    for (auto i = head->getNext(); i != head; i = i->getNext())
    {
        i->output();
    }
}

void BasicBlock::addSucc(BasicBlock *bb)
{
    succ.push_back(bb);
}

void BasicBlock::removeSucc(BasicBlock *bb)
{
    succ.erase(std::find(succ.begin(), succ.end(), bb));
}

void BasicBlock::addPred(BasicBlock *bb)
{
    pred.push_back(bb);
}

void BasicBlock::removePred(BasicBlock *bb)
{
    pred.erase(std::find(pred.begin(), pred.end(), bb));
}
void BasicBlock::genMachineCode(AsmBuilder *builder)
{
    auto cur_func = builder->getFunction();
    auto cur_block = new MachineBlock(cur_func, no);
    builder->setBlock(cur_block);
    for (auto i = head->getNext(); i != head; i = i->getNext())
    {
        i->genMachineCode(builder);
    }
    cur_func->InsertBlock(cur_block);
}

BasicBlock::BasicBlock(Function *f)
{
    this->no = SymbolTable::getLabel();
    f->insertBlock(this);
    parent = f;
    head = new DummyInstruction();
    head->setParent(this);
}

BasicBlock::~BasicBlock()
{
    Instruction *inst;
    inst = head->getNext();
    while (inst != head)
    {
        Instruction *t;
        t = inst;
        inst = inst->getNext();
        delete t;
    }
    for (auto &bb : pred)
    {
        bb->removeSucc(this);
    }
    for (auto &bb : succ)
    {
        bb->removePred(this);
    }
    parent->remove(this);
}

void BasicBlock::cleanSucc()
{
    for (auto i : succ)
    {
        i->removePred(this);
    }
    vector<BasicBlock *>().swap(succ);
}