#include<cstring>
#include "../include/symtab.h"

SymbolTable::SymbolTable(Scope* r)
{
    if(r == nullptr)
    {
        Scope* temp = new Scope;
        root = temp;
    }
    else
        root = r;
    root->parent = nullptr;
    curr = root;
}

void SymbolTable::addChild(Scope* parent,Scope* child)
{
    parent->children.push_back(child);
}

void SymbolTable::addSymbol(Symbol* sym, Scope* sc)
{

    sc->symbols.push_back(sym);
}

void SymbolTable::retParent()
{
    curr = curr->parent;
}

Scope* SymbolTable::getParent(Scope* child)
{
    return child->parent;
}

Scope* SymbolTable::findScope(char* name, Scope* sc)
{
    Scope* curr = sc;
    if(curr != nullptr)
    {
        std::vector<Symbol*>::iterator it = sc->symbols.begin();
        while (it != sc->symbols.end())
        {
            if(!strcmp((*it)->name, name)) 
                return curr;
            it++;
        }
        curr = curr->parent;
        if(curr == nullptr) 
            return nullptr;
    }
    return nullptr;
}
