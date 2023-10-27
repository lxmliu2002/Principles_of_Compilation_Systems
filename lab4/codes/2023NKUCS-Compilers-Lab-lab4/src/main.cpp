#include <iostream>
#include <string.h>
#include <unistd.h>
#include "common.h"
#include "Ast.h"
#include "Type.h"
#include "SymbolTable.h"

using namespace std;

Ast ast;
extern FILE *yyin;
extern FILE *yyout;

int yyparse();

char outfile[256] = "a.out";
dump_type_t dump_type = ASM;

int main(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "ato:")) != -1)
    {
        switch (opt)
        {
        case 'o':
            strcpy(outfile, optarg);
            break;
        case 'a':
            dump_type = AST;
            break;
        case 't':
            dump_type = TOKENS;
            break;
        default:
            fprintf(stderr, "Usage: %s [-o outfile] infile\n", argv[0]);
            exit(EXIT_FAILURE);
            break;
        }
    }
    if (optind >= argc)
    {
        fprintf(stderr, "no input file\n");
        exit(EXIT_FAILURE);
    }
    if (!(yyin = fopen(argv[optind], "r")))
    {
        fprintf(stderr, "%s: No such file or directory\nno input file\n", argv[optind]);
        exit(EXIT_FAILURE);
    }
    if (!(yyout = fopen(outfile, "w")))
    {
        fprintf(stderr, "%s: fail to open output file\n", outfile);
        exit(EXIT_FAILURE);
    }
    printf("%s\n", argv[optind]);
    Type *funcType;
    funcType = new FunctionType(TypeSystem::intType, {});
    SymbolEntry *se = new IdentifierSymbolEntry(funcType, "getint", identifiers->getLevel());
    identifiers->install("getint", se);

    // getch
    funcType = new FunctionType(TypeSystem::intType, {});
    se = new IdentifierSymbolEntry(funcType, "getch", identifiers->getLevel());
    identifiers->install("getch", se);

    // getfloat
    funcType = new FunctionType(TypeSystem::floatType, {});
    se = new IdentifierSymbolEntry(funcType, "getfloat", identifiers->getLevel());
    identifiers->install("getfloat", se);

    // getarray
    funcType = new FunctionType(TypeSystem::intType, {});
    se = new IdentifierSymbolEntry(funcType, "getarray", identifiers->getLevel());
    identifiers->install("getarray", se);

    // putint
    funcType = new FunctionType(TypeSystem::voidType, {});
    se = new IdentifierSymbolEntry(funcType, "putint", identifiers->getLevel());
    identifiers->install("putint", se);

    // putch
    funcType = new FunctionType(TypeSystem::voidType, {});
    se = new IdentifierSymbolEntry(funcType, "putch", identifiers->getLevel());
    identifiers->install("putch", se);

    // putfloat
    funcType = new FunctionType(TypeSystem::voidType, {});
    se = new IdentifierSymbolEntry(funcType, "putfloat", identifiers->getLevel());
    identifiers->install("putfloat", se);

    // putarray
    funcType = new FunctionType(TypeSystem::voidType, {});
    se = new IdentifierSymbolEntry(funcType, "putarray", identifiers->getLevel());
    identifiers->install("putarray", se);

    // putf
    funcType = new FunctionType(TypeSystem::voidType, {});
    se = new IdentifierSymbolEntry(funcType, "putf", identifiers->getLevel());
    identifiers->install("putf", se);

    // starttime
    funcType = new FunctionType(TypeSystem::voidType, {});
    se = new IdentifierSymbolEntry(funcType, "starttime", identifiers->getLevel());
    identifiers->install("starttime", se);

    // stoptime
    funcType = new FunctionType(TypeSystem::voidType, {});
    se = new IdentifierSymbolEntry(funcType, "stoptime", identifiers->getLevel());
    identifiers->install("stoptime", se);

    yyparse();
    if(dump_type == AST)
        ast.output();
    return 0;
}
