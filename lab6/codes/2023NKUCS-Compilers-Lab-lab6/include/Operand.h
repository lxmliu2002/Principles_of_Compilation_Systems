#ifndef __OPERAND_H__
#define __OPERAND_H__

#include <vector>
#include "SymbolTable.h"

class Instruction;
class Function;

class Operand
{
    typedef std::vector<Instruction *>::iterator use_iterator;

private:
    Instruction *def;
    std::vector<Instruction *> uses;
    SymbolEntry *se;

public:
    Operand(SymbolEntry *se) : se(se) { def = nullptr; };
    void setDef(Instruction *inst) { def = inst; };
    void addUse(Instruction *inst) { uses.push_back(inst); };
    void removeUse(Instruction *inst);
    int usersNum() const { return uses.size(); };

    use_iterator use_begin() { return uses.begin(); };
    use_iterator use_end() { return uses.end(); };
    Type *getType() { return se->getType(); };
    std::string toStr() const;
    SymbolEntry *getEntry() { return se; };
    void setEntry(SymbolEntry *se) { this->se = se; };
    Instruction *getDef() { return def; };
};

#endif