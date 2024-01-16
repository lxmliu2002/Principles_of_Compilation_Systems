#ifndef __TYPE_H__
#define __TYPE_H__
#include <vector>
#include <string>

using namespace std;

class Type
{
private:
    int kind;

protected:
    enum
    {
        INT,
        FLOAT,
        VOID,
        FUNC,
        ARRAY,
        STRING,
        PTR
    };

public:
    Type(int kind) : kind(kind){};
    virtual ~Type(){};
    virtual std::string toStr() = 0;
    bool isInt() const { return kind == INT; };
    bool isFloat() const { return kind == FLOAT; };
    bool isVoid() const { return kind == VOID; };
    bool isFunc() const { return kind == FUNC; };
    bool isArray() const { return kind == ARRAY; };
    bool isString() const { return kind == STRING; };
    bool isPtr() const { return kind == PTR; }
    static bool isValid(Type *t1, Type *t2);
};

class IntType : public Type
{
private:
    int size;
    bool constant;

public:
    IntType(int size, bool constant = false)
        : Type(Type::INT), size(size), constant(constant){};
    string toStr();
    bool isConstant() { return constant; }
};

class FloatType : public Type
{
private:
    float size;
    bool constant;

public:
    FloatType(float size, bool constant = false)
        : Type(Type::FLOAT), size(size), constant(constant){};
    string toStr();
    bool isConstant() { return constant; }
};

class VoidType : public Type
{
public:
    VoidType() : Type(Type::VOID){};
    std::string toStr();
};

class FunctionType : public Type
{
private:
    Type *returnType;          // 返回值类型
    vector<Type *> paramsType; // 参数类型
public:
    FunctionType(Type *returnType, vector<Type *> paramsType) : Type(Type::FUNC), returnType(returnType), paramsType(paramsType){};
    void setParamsType(vector<Type *> p) { paramsType = p; }
    Type *getParamsType(unsigned int index)
    {
        if (index < paramsType.size())
            return paramsType[index];
        else
            return nullptr;
    }
    Type *getRetType() { return returnType; };
    std::string toStr();
};

class PointerType : public Type
{
private:
    Type *valueType;

public:
    PointerType(Type *valueType) : Type(Type::PTR) { this->valueType = valueType; };
    Type *getValueType() { return valueType; }
    std::string toStr();
};

class ArrayType : public Type
{
private:
    Type *elementType;
    Type *arrayType = nullptr;
    int length;
    bool constant;

public:
    ArrayType(Type *elementType, int length, bool constant = false)
        : Type(Type::ARRAY), elementType(elementType), length(length), constant(constant){};
    string toStr();
    int getLength() const { return length; };
    Type *getElementType() const { return elementType; };
    void setArrayType(Type *arrayType) { this->arrayType = arrayType; };
    Type *getArrayType() const { return arrayType; };
};

class StringType : public Type
{
private:
    int length;

public:
    StringType(int length) : Type(Type::STRING), length(length){};
    int getLength() const { return length; };
    string toStr();
};

class TypeSystem
{
private:
    static IntType commonInt;
    static FloatType commonFloat;
    static IntType commonBool;
    static VoidType commonVoid;

public:
    static Type *intType;
    static Type *floatType;
    static Type *voidType;
    static Type *boolType;
};

#endif
