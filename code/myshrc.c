#include "myshrc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ctype.h>
#include <sys/types.h>

aliasList getAliases() {
    aliasList aliasHead = NULL;

    FILE* myshrcFile = fopen("systemFiles/.myshrc", "r");
    if (!myshrcFile) {
        return NULL;
    }

    
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, myshrcFile)) != -1) {
        size_t lineLen = strlen(line);
        if (lineLen > 0 && line[lineLen - 1] == '\n') {
            line[lineLen - 1] = '\0';
        }
            
        // check if first word is alias
        if (!strncmp(line, "alias", 5)) {
            line += 5;

            char* alias = strtok(line, "=");
            char* command = strtok(NULL, "=");
            if (alias && command) {
                aliases* newAlias = (aliases*) malloc(sizeof(aliases));
                newAlias->alias = prune(strdup(alias));
                newAlias->command = prune(strdup(command));
                newAlias->next = aliasHead;
                aliasHead = newAlias;
        printf("\033[1;33m Alias: [%s], Command: [%s]\033[0m\n", aliasHead->alias, aliasHead->command);
            }

            line -= 5;
        }
    }
    fclose(myshrcFile);
    if (line) {
        free(line);
    }
    return aliasHead;
}

funcList getFuncs() {
    funcList funcHead = NULL;

    FILE* myshrcFile = fopen("systemFiles/.myshrc", "r");
    if (!myshrcFile) {
        return NULL;
    }

    
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, myshrcFile)) != -1) {
        size_t lineLen = strlen(line);
        if (lineLen > 0 && line[lineLen - 1] == '\n') {
            line[lineLen - 1] = '\0';
        }
            
        // Check if first word is func
        if (!strncmp(line, "func", 4)) {
            line += 4;

            char* func = prune(strdup(strtok(line, "(")));
            char* cmnd = (char*) malloc(1024);
            while ((read = getline(&line, &len, myshrcFile)) != -1) {
                size_t lineLen = strlen(line);
                if (lineLen > 0 && line[lineLen - 1] == '\n') {
                    line[lineLen - 1] = '\0';
                }
                if (strncmp(line, "}", 1) == 0) {
                    break;
                }
                if (cmnd[0] != '\0') {
                    strcat(cmnd, " ; ");
                }
                strcat(cmnd, prune(line));
            }

            funcs* newFunc = (funcs*) malloc(sizeof(funcs));
            newFunc->func = strdup(func);
            newFunc->command = strdup(cmnd);
            newFunc->next = funcHead;
            funcHead = newFunc;
            printf("\033[1;33m Function: [%s], Command: [%s]\033[0m\n", funcHead->func, funcHead->command);

            line -= 4;
        }
    }

    fclose(myshrcFile);
    if (line) {
        free(line);
    }
    return funcHead;
}

// Remove leading and trailing whitespaces
char* prune(char* str) {
    char* end;
    while (isspace((unsigned char)*str)) {
        str++;
    }
    if (*str == 0) {
        return str;
    }
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }
    end[1] = '\0';
    return str;
}
