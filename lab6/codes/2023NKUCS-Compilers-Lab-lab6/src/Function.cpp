#include "Function.h"
#include <list>
#include <vector>
#include "Type.h"
#include "Unit.h"

using namespace std;

extern FILE *yyout;

Function::Function(Unit *u, SymbolEntry *s)
{
    u->insertFunc(this);
    entry = new BasicBlock(this);
    sym_ptr = s;
    parent = u;
}

Function::~Function()
{
}

void Function::remove(BasicBlock *bb)
{
    block_list.erase(std::find(block_list.begin(), block_list.end(), bb));
}

void Function::output() const
{
    FunctionType *funcType = dynamic_cast<FunctionType *>(sym_ptr->getType());
    Type *retType = funcType->getRetType();
    std::vector<SymbolEntry *> paramsSe = funcType->getParamsSe();
    if (!paramsSe.size())
    {
        fprintf(yyout, "define %s %s() {\n", retType->toStr().c_str(), sym_ptr->toStr().c_str());
    }
    else
    {
        fprintf(yyout, "define %s %s(", retType->toStr().c_str(), sym_ptr->toStr().c_str());
        for (long unsigned int i = 0; i < paramsSe.size(); i++)
        {
            if (i)
            {
                fprintf(yyout, ", ");
            }
            fprintf(yyout, "%s %s", paramsSe[i]->getType()->toStr().c_str(), paramsSe[i]->toStr().c_str());
        }
        fprintf(yyout, ") {\n");
    }
    std::set<BasicBlock *> v;
    std::list<BasicBlock *> q;
    q.push_back(entry);
    v.insert(entry);

    while (!q.empty())
    {
        auto bb = q.front();
        q.pop_front();
        bb->output();
        for (auto succ = bb->succ_begin(); succ != bb->succ_end(); succ++)
        {
            if (v.find(*succ) == v.end())
            {
                v.insert(*succ);
                q.push_back(*succ);
            }
        }
    }
    fprintf(yyout, "}\n");
}

void Function::genMachineCode(AsmBuilder *builder)
{
    auto cur_unit = builder->getUnit();
    auto cur_func = new MachineFunction(cur_unit, this->sym_ptr);
    builder->setFunction(cur_func);
    std::map<BasicBlock *, MachineBlock *> map;
    for (auto block : block_list)
    {
        block->genMachineCode(builder);
        map[block] = builder->getBlock();
    }
    for (auto block : block_list)
    {
        auto mblock = map[block];
        for (auto pred = block->pred_begin(); pred != block->pred_end(); pred++)
        {
            mblock->addPred(map[*pred]);
        }
        for (auto succ = block->succ_begin(); succ != block->succ_end(); succ++)
        {
            mblock->addSucc(map[*succ]);
        }
    }
    cur_unit->InsertFunc(cur_func);
}

vector<vector<int>> Function::getBlockMap()
{
    int len = block_list.size();
    vector<vector<int>> m(len, vector<int>(len, 0));
    for (int i = 0; i < len; i++)
    {
        auto iter = block_list[i]->succ_begin();
        auto end = block_list[i]->succ_end();
        while (iter != end)
        {
            int j = find(block_list.begin(), block_list.end(), *iter) - block_list.begin();
            m[i][j] = 1;
            iter++;
        }
    }
    return m;
}