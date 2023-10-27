#include<cstring>
#include<iostream>
#include "../include/symtab.h"

SymbolTable::SymbolTable(Scope* r)
{
    if(r == nullptr)
    {
        Scope* temp = new Scope;
        root = temp;
    }
    else
    {
        root = r;
    }
    root->parent = nullptr;
    curr = root;
}

void SymbolTable::addChild(Scope* parent,Scope* child)
{
    child->parent = parent;
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
    Scope* temp = sc;
    while(temp != nullptr)
    {
        std::vector<Symbol*>::iterator it = temp->symbols.begin();
        while (it != temp->symbols.end())
        {
            if(!strcmp((*it)->name, name)) 
                return temp;
            it++;
        }
        temp = temp->parent;
    }
    return nullptr;
}

Symbol* SymbolTable::getSymbol(char* name, Scope* sc)
{
    for(auto s : sc->symbols)
    {
        if(strcmp(name, s->name) == 0)
            return s;
    }
    return nullptr;
}
/*
SymbolTable* SymbolTable::findSymbolTable(char* name, SymbolTable* sy = nullptr)
{
    SymbolTable* curr = nullptr;

    Scope* sc = 
}
*/