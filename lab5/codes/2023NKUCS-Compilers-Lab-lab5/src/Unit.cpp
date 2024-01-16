#include "Unit.h"

extern FILE *yyout;

void Unit::insertFunc(Function *f)
{
    func_list.push_back(f);
}

void Unit::removeFunc(Function *func)
{
    func_list.erase(std::find(func_list.begin(), func_list.end(), func));
}

void Unit::output() const
{
    fprintf(yyout, "target triple = \"x86_64-pc-linux-gnu\"\n\n");
    fprintf(yyout, "declare i32 @getint()\n");
    fprintf(yyout, "declare void @putint(i32)\n");
    fprintf(yyout, "declare i32 @getch()\n");
    fprintf(yyout, "declare void @putch(i32)\n");
    fprintf(yyout, "declare void @putf(i32)\n\n");
    for (auto i : global_var)
        i->output();
    for (auto &func : func_list)
        func->output();
}

Unit::~Unit()
{
    auto delete_var = global_var;
    for (auto &i : delete_var)
        delete i;
    auto delete_list = func_list;
    for (auto &func : delete_list)
        delete func;
}
