#include<vector>
#include<cstring>


struct Symbol {
    char* name;
};

struct Scope {
    Scope* parent;
    std::vector<Scope*> children;
    std::vector<Symbol*> symbols;
};

class SymbolTable
{
public:
    Scope* root; // 根节点（全局变量）
    const char reserve[10][15] = {{"const"},{"int"},{"float"},{"void"},{"if"},{"else"},{"while"},{"break"},{"continue"},{"return"}}; // 保留字
    Scope* curr; // 当前作用域
    SymbolTable(Scope* r = nullptr);
    void addChild(Scope* parent, Scope* child);
    void addSymbol(Symbol* sym, Scope* sc);
    void retParent();
    Scope* getParent(Scope* child);
    Scope* findScope(char* name, Scope* sc = nullptr);
    Symbol* getSymbol(char* name, Scope* sc);
    //SymbolTable* SymbolTable::findSymbolTable(char* name, SymbolTable* sy = nullptr);
    ~SymbolTable(){};
}; 

