#ifndef __IRBUILDER_H__
#define __IRBUILDER_H__

class Unit;
class Function;
class BasicBlock;

class IRBuilder
{
private:
    Unit *unit;
    BasicBlock *insertBB; // The current basicblock that instructions should be inserted into.
    // StmtNode* whileContentStmt;
    std::vector<Instruction *> while_true_list;
    std::vector<Instruction *> while_false_list;

public:
    IRBuilder(Unit *unit) : unit(unit){};
    void setInsertBB(BasicBlock *bb) { insertBB = bb; };
    Unit *getUnit() { return unit; };
    BasicBlock *getInsertBB() { return insertBB; };
    // void setWhileContentStmt(StmtNode* stmt){whileContentStmt = stmt;}
    // StmtNode* getWhileContentStmt(){return whileContentStmt;}
    void addWhileList(Instruction *i, bool isTrue)
    {
        if (isTrue)
            while_true_list.push_back(i);
        else
            while_false_list.push_back(i);
    }
    std::vector<Instruction *> &getWhileList(bool isTrue)
    {
        if (isTrue)
            return while_true_list;
        else
            return while_false_list;
    }
};

#endif