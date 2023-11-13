#include "Type.h"
#include <sstream>
#include <assert.h>
#include <iostream>

using namespace std;

IntType TypeSystem::commonInt = IntType(4);
FloatType TypeSystem::commonFloat = FloatType(4);
VoidType TypeSystem::commonVoid = VoidType();
IntType TypeSystem::commonConstInt = IntType(4, true);

Type* TypeSystem::intType = &commonInt;
Type* TypeSystem::floatType = &commonFloat;
Type* TypeSystem::voidType = &commonVoid;
// Type *TypeSystem::constIntType = &commonConstInt;

string IntType::toStr()
{
    if (constant)
    {
        return "constant int";
    }
    else
    {
        return "int";
    }
}

string FloatType::toStr()
{
    if (constant)
    {
        return "constant float";
    }
    else
    {
        return "float";
    }
}

string VoidType::toStr()
{
    return "void";
}

string ArrayType::toStr()
{
    vector<string> vec;
    Type *temp = this;
    while (temp && temp->isArray())
    {
        ostringstream buffer;
        if (temp == this && length == 0)
        {    
            buffer << '[' << ']';
        }
        else
        {    
            buffer << '[' << ((ArrayType *)temp)->getLength() << ']';
        }
        vec.push_back(buffer.str());
        temp = ((ArrayType *)temp)->getElementType();
        ;
    }
    ostringstream buffer;
    if (constant)
    {    
        buffer << "const ";
    }
    if(temp->isInt())
    {
        buffer << "int";
    }
    else if(temp->isFloat())
    {
        buffer << "float";
    }
    for (auto it = vec.begin(); it != vec.end(); it++)
    {    
        buffer << *it;
    }
    return buffer.str();
}

string FunctionType::toStr()
{
    ostringstream buffer;
    buffer << returnType->toStr() << "(";
    for (auto it = paramsType.begin(); it != paramsType.end(); it++)
    {
        buffer << (*it)->toStr();
        if (it + 1 != paramsType.end())
        {    
            buffer << ", ";
        }
    }
    buffer << ')';
    return buffer.str();
}

string StringType::toStr()
{
    ostringstream buffer;
    buffer << "const char[" << length << "]";
    return buffer.str();
}
