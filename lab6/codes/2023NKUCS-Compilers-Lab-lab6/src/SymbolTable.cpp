#include "SymbolTable.h"
#include <iostream>
#include <sstream>
#include "Type.h"

bool SymbolEntry::setNext(SymbolEntry *se)
{
    SymbolEntry *s = this;
    long unsigned int cnt = ((FunctionType *)(se->getType()))->getParamsType().size();
    if (cnt == ((FunctionType *)(s->getType()))->getParamsType().size())
    {
        return false;
    }
    while (s->getNext())
    {
        if (cnt == ((FunctionType *)(s->getType()))->getParamsType().size())
        {
            return false;
        }
        s = s->getNext();
    }
    if (s == this)
    {
        this->next = se;
    }
    else
    {
        s->setNext(se);
    }
    return true;
}

SymbolEntry::SymbolEntry(Type *type, int kind)
{
    this->type = type;
    this->kind = kind;
}

ConstantSymbolEntry::ConstantSymbolEntry(Type *type, double value) : SymbolEntry(type, SymbolEntry::CONSTANT)
{
    assert(type->isInt() || type->isFloat());
    this->value = value;
}

ConstantSymbolEntry::ConstantSymbolEntry(Type *type, std::string value) : SymbolEntry(type, SymbolEntry::CONSTANT)
{
    assert(type->isString());
    this->strValue = value;
}

ConstantSymbolEntry::ConstantSymbolEntry(Type *type) : SymbolEntry(type, SymbolEntry::CONSTANT)
{
}

double ConstantSymbolEntry::getValue() const
{
    assert(type->isInt() || type->isFloat());
    return value;
}

std::string ConstantSymbolEntry::getStrValue() const
{
    assert(type->isString());
    return strValue;
}

std::string ConstantSymbolEntry::toStr()
{
    std::ostringstream buffer;
    if (type->isInt())
    {
        buffer << (int)value;
    }
    else if (type->isFloat())
    {
        buffer << value;
    }
    else if (type->isString())
    {
        buffer << strValue;
    }
    return buffer.str();
}

IdentifierSymbolEntry::IdentifierSymbolEntry(Type *type, std::string name, int scope, int paramNo, bool sysy) : SymbolEntry(type, SymbolEntry::VARIABLE), name(name), sysy(sysy), paramNo(paramNo)
{
    this->scope = scope;
    this->initial = false;
    this->label = -1;
    this->allZero = false;
    this->constant = false;
}

void IdentifierSymbolEntry::setValue(double value)
{
    if (((IntType *)(this->getType()))->isConst())
    {
        if (!initial)
        {
            this->value = value;
            initial = true;
        }
        else
        {
            fprintf(stderr, "trying to set value for constant.\n");
        }
    }
    else
    {
        this->value = value;
    }
}

void IdentifierSymbolEntry::setArrayValue(double *arrayValue)
{
    if (((ArrayType *)(this->getType()))->isConst())
    {
        if (!initial)
        {
            this->arrayValue = arrayValue;
            initial = true;
        }
        else
        {
            fprintf(stderr, "trying to set value for constant.\n");
        }
    }
    else
    {
        this->arrayValue = arrayValue;
    }
}

std::string IdentifierSymbolEntry::toStr()
{
    std::ostringstream buffer;
    if (label < 0)
    {
        if (type->isFunc())
        {
            buffer << '@';
        }
        buffer << name;
    }
    else
    {
        buffer << "%t" << label;
    }
    return buffer.str();
}

TemporarySymbolEntry::TemporarySymbolEntry(Type *type, int label) : SymbolEntry(type, SymbolEntry::TEMPORARY)
{
    this->label = label;
}

std::string TemporarySymbolEntry::toStr()
{
    std::ostringstream buffer;
    buffer << "%t" << label;
    return buffer.str();
}

SymbolTable::SymbolTable()
{
    prev = nullptr;
    level = 0;
}

SymbolTable::SymbolTable(SymbolTable *prev)
{
    this->prev = prev;
    this->level = prev->level + 1;
}

SymbolEntry *SymbolTable::lookup(std::string name)
{
    SymbolTable *table = this;
    while (table != nullptr)
        if (table->symbolTable.find(name) != table->symbolTable.end())
        {
            return table->symbolTable[name];
        }
        else
        {
            table = table->prev;
        }
    return nullptr;
}

bool SymbolTable::install(std::string name, SymbolEntry *entry)
{
    if (this->symbolTable.find(name) != this->symbolTable.end())
    {
        SymbolEntry *se = this->symbolTable[name];
        if (se->getType()->isFunc())
        {
            return se->setNext(entry);
        }
        return false;
    }
    else
    {
        symbolTable[name] = entry;
        return true;
    }
}

int SymbolTable::counter = 0;
static SymbolTable t;
SymbolTable *identifiers = &t;
SymbolTable *globals = &t;
