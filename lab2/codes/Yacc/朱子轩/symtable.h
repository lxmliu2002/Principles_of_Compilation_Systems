#ifndef NSYMS
#define NSYMS 30
#endif

struct symtable {
	char *name;
	int value;
}symtable[NSYMS] = {};

int lookup(char* s);
