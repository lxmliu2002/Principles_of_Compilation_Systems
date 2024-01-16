#include "ElimComSubexpr.h"
#include <set>
#include <algorithm>
#include <queue>
typedef vector<BasicBlock *>::iterator bb_iterator;
typedef vector<struct aeb> VAEB;//存储值编号表达式
using namespace std;
const int N = 2000100;
bool ste[N];
unordered_map<int, VAEB> AEin, AEout;



void delAEB(SymbolEntry *sym, VAEB AEB)//从值编号表达式列表中删除与给定符号相关的项
{
    int len = AEB.size();
    for (int i = 0; i < len; i++)
    {
        if (sym == AEB[i].opd1 || sym == AEB[i].opd2)
        {
            for (int j = i; j < len - 1; j++)
            {
                AEB[j] = AEB[j + 1];
            }
            len--;
        }
    }
}

VAEB vec_intersection(VAEB v1, VAEB v2)//交集
{
    VAEB v11, v22, res;
    int len = min(v1.size(), v2.size());
    if (v1.size() > v2.size())
    {
        v11 = v2;
        v22 = v1;
    }
    else
    {
        v11 = v1;
        v22 = v2;
    }
    VAEB::iterator it;
    for (int i = 0; i < len; i++)
    {
        it = find(v22.begin(), v22.end(), v11[i]);
        if (it != v22.end())
        {
            res.push_back(v11[i]);
        }
    }
    return res;
}

VAEB vec_union(VAEB v1, VAEB v2)//并集
{
    VAEB res(v1);
    VAEB::iterator it;
    for (unsigned i = 0; i < v2.size(); i++)
    {
        it = find(v1.begin(), v1.end(), v2[i]);
        if (it == v1.end())
        {
            res.push_back(v2[i]);
        }
    }
    return res;
}

VAEB vintersection(BasicBlock *bb)//bb 的入口值编号表达式列表的交集
{
    VAEB res;
    bb_iterator iter = bb->pred_begin();
    bb_iterator end = bb->pred_end();
    while (iter != end)
    {
        int no = (*iter)->getNo();
        if (ste[no])
        {
            res = vec_intersection(res, AEout[no]);
        }
        iter++;
    }
    return res;
}

void ElimComSubexpr::local_elim_cse(BasicBlock *bb, VAEB AEB)
{
    int no = bb->getNo();
    for (auto iter = bb->begin(); iter != bb->end(); iter = iter->getNext())//遍历
    {
        vector<Operand *> operands(iter->getOperands());
        if (iter->isBin())//二元指令
        {
            int op = ((BinaryInstruction *)iter)->getOp();
            Instruction *p = iter;
            bool found = false;

            SymbolEntry *sym1 = operands[1]->getEntry(), *sym2 = operands[2]->getEntry();
            if (((IdentifierSymbolEntry *)sym1)->isSysy() || ((IdentifierSymbolEntry *)sym2)->isSysy())//系统指令
            {
                continue;
            }
            if (!sym1->isConstant())
            {
                auto iter_def = operands[1]->getDef();
                if (iter_def)
                {
                    vector<Operand *> def_operands(iter_def->getOperands());
                    sym1 = def_operands[1]->getEntry();//获取定义指令的第二个操作数的符号
                }
            }
            if (!sym2->isConstant())
            {
                auto iter_def = operands[2]->getDef();
                if (iter_def)
                {
                    vector<Operand *> def_operands(iter_def->getOperands());
                    sym2 = def_operands[1]->getEntry();
                }
            }
            int len = AEB.size();
            int i = 0;
            for (; i < len; i++)//遍历查找共同子表达式
            {
                if (op == AEB[i].opr && sym1->toStr() == AEB[i].opd1->toStr() && sym2->toStr() == AEB[i].opd2->toStr())
                {
                    found = true;
                    break;
                }
            }
            if (found)
            {
                p = AEB[i].inst;
                Operand *dst = AEB[i].tmp;
                if (dst == nullptr)
                {
                    dst = new Operand(new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel()));
                    vector<Operand *> pOperands(p->getOperands());
                    Instruction *inst = new BinaryInstruction(op, dst, pOperands[1], pOperands[2], nullptr);
                    AEB[i].tmp = dst;
                    bb->insertBefore(inst, p);
                    Instruction *inst1 = new LoadInstruction(pOperands[0], dst, nullptr);
                    bb->insertBefore(inst1, p);
                    bb->remove(p);
                }
                Instruction *inst2 = new LoadInstruction(operands[0], dst, nullptr);
                bb->insertBefore(inst2, iter);
                bb->remove(iter);
            }
            else
            {
                struct aeb tmp;
                tmp.inst = (BinaryInstruction *)iter, tmp.opd1 = sym1, tmp.opr = op, tmp.opd2 = sym2;
                AEB.push_back(tmp);
                SymbolEntry *sym = operands[0]->getEntry();
                auto iter_def = operands[0]->getDef();
                if (iter_def)
                {
                    vector<Operand *> def_operands(iter_def->getOperands());
                    sym = def_operands[1]->getEntry();
                }
                delAEB(sym, AEB);
                delAEB(sym, AEin[no]);
            }
        }
    }
    AEout[no] = vec_union(AEB, AEin[no]);
    AEB.clear();
}

void ElimComSubexpr::elim_cse()
{
    auto iter = unit->begin();
    VAEB AEB;
    while (iter != unit->end())//遍历
    {
        vector<BasicBlock *> block_list = (*iter)->getBlockList();//列表
        BasicBlock *bb = (*iter)->getEntry();//入口点
        queue<BasicBlock *> q;
        q.push(bb);
        bool first = true;
        while (!q.empty())//宽度优先搜索遍历基本块
        {
            BasicBlock *bb = q.front();
            q.pop();
            int no = bb->getNo();
            ste[no] = true;
            if (first)
            {
                AEin[no];
                first = false;
            }
            else
            {
                AEin[no] = vintersection(bb);
            }
            local_elim_cse(bb, AEB);//局部删除
            for (auto succ = bb->succ_begin(); succ != bb->succ_end(); succ++)
            {
                if (!ste[(*succ)->getNo()])
                {
                    q.push(*succ);
                }
            }
        }
        iter++;
    }
}


ElimComSubexpr::~ElimComSubexpr()
{
    AEin.clear(), AEout.clear();
}