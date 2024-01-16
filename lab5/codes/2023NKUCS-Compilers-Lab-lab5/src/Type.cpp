#include "Type.h"
#include <sstream>

IntType TypeSystem::commonInt = IntType(32);
FloatType TypeSystem::commonFloat = FloatType(32);
IntType TypeSystem::commonBool = IntType(1);
VoidType TypeSystem::commonVoid = VoidType();

Type *TypeSystem::intType = &commonInt;
Type *TypeSystem::floatType = &commonFloat;
Type *TypeSystem::voidType = &commonVoid;
Type *TypeSystem::boolType = &commonBool;

bool Type::isValid(Type *t1, Type *t2)
{
    if (t1->isFloat() || t2->isFloat() || t1->isInt() || t2->isInt() || t1 == t2)
        return true;
    return false;
}

std::string IntType::toStr()
{
    std::ostringstream buffer;
    buffer << "i" << size;
    return buffer.str();
}

string FloatType::toStr()
{
    std::ostringstream buffer;
    buffer << "float";
    return buffer.str();
}

std::string VoidType::toStr()
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
    if (temp->isInt())
    {
        buffer << "int";
    }
    else if (temp->isFloat())
    {
        buffer << "float";
    }
    for (auto it = vec.begin(); it != vec.end(); it++)
    {
        buffer << *it;
    }
    return buffer.str();
}

std::string FunctionType::toStr()
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

std::string PointerType::toStr()
{
    std::ostringstream buffer;
    buffer << valueType->toStr() << "*";
    return buffer.str();
}

string StringType::toStr()
{
    ostringstream buffer;
    buffer << "const char[" << length << "]";
    return buffer.str();
}