#ifndef __OPERAND_H__
#define __OPERAND_H__

#include "SymbolTable.h"
#include <vector>

class Instruction;
class Function;

// class Operand - The operand of an instruction.
class Operand
{
    typedef std::vector<Instruction *>::iterator use_iterator;

private:
    Instruction *def;                // The instruction where this operand is defined.
    std::vector<Instruction *> uses; // Intructions that use this operand.
    SymbolEntry *se;                 // The symbol entry of this operand.
public:
    Operand(SymbolEntry *se) : se(se) { def = nullptr; };
    void setDef(Instruction *inst) { def = inst; };
    void addUse(Instruction *inst) { uses.push_back(inst); };
    void removeUse(Instruction *inst);
<<<<<<< HEAD
    int usersNum() const { return uses.size(); };

    use_iterator use_begin() { return uses.begin(); };
    use_iterator use_end() { return uses.end(); };
    Type *getType() { return se->getType(); };
=======
    bool isConst(){if(!se->isVariable()) return false; else return ((IdentifierSymbolEntry*)se)->isConst();}
    int usersNum() const {return uses.size();};
    SymbolEntry* getSymPtr(){return se;}
    use_iterator use_begin() {return uses.begin();};
    use_iterator use_end() {return uses.end();};
    Type* getType() {return se->getType();};
>>>>>>> fb5c90b429e0844b7b29767f93558e20029563fb
    std::string toStr() const;
};

#endif