#include<vector>

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
    Scope* root;
    const char reserve[9][20] = {{"int"},{"float"},{"void"},{"if"},{"else"},{"while"},{"break"},{"continue"},{"return"}};
    Scope* curr;
    SymbolTable(Scope* r = nullptr);
    void addChild(Scope* parent, Scope* child);
    void addSymbol(Symbol* sym, Scope* sc);
    void retParent();
    Scope* getParent(Scope* child);
    Scope* findScope(char* name, Scope* sc = nullptr);
    ~SymbolTable(){};
}; 

