#include "symtable.h"
#include <string.h>

int lookup(char* s){
    for(int i = 0; i < NSYMS; i++) {
        if(symtable[i].name && !strcmp(s,symtable[i].name)) return i;
        if(!symtable[i].name) {
            symtable[i].name = strdup(s);
            return i;
        }
    }
    return -1;
}

