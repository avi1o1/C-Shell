#ifndef MYSHRC_H
#define MYSHRC_H

typedef struct aliases {
    char* alias;
    char* command;
    struct aliases* next;
} aliases;
typedef aliases* aliasList;

typedef struct funcs {
    char* func;
    char* command;
    struct funcs* next;
} funcs;
typedef funcs* funcList;

// Get aliases from .myshrc
aliasList getAliases();

// Get functions from .myshrc
funcList getFuncs();

// Remove leading and trailing whitespaces
char* prune(char* str);

#endif