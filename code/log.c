#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to get logs of the shell
char* notTheMathLog(char* cmndTokens, char* logFilePath) {
    // In case of no parameters, print the logs
    if (!cmndTokens) {
        FILE *logFile = fopen(logFilePath, "r");
        if (logFile == NULL) {
            printf("\033[1;31m Oopsie Woopsie : Log file not found! \033[0m\n");
            return "";
        }
        
        char c;
        while ((c = fgetc(logFile)) != EOF) {
            printf("%c", c);
        }
        fclose(logFile);
        return "";
    }

    // In case of "purge", clear the logs
    else if (!strcmp(cmndTokens, "purge")) {
        FILE *logFile = fopen(logFilePath, "w");
        if (logFile == NULL) {
            printf("\033[1;31m Oopsie Woopsie : Log file not found! \033[0m\n");
            return "";
        }
        fclose(logFile);
        return "";
    }

    // In case of "execute", return the value-th command from the logs
    else if (!strcmp(cmndTokens, "execute")) {
        // Check for the validity of the value
        char* nxtCmnd = (char*) malloc(1024);
        cmndTokens = strtok(NULL, " \t");
        if (!cmndTokens) {
            printf("\033[1;31m Oopsie Woopsie : No value provided! \033[0m\n");
            return "";
        }
        int value = atoi(cmndTokens);

        // Open the log file
        FILE *logFile = fopen(logFilePath, "r");
        if (logFile == NULL) {
            printf("\033[1;31m Oopsie Woopsie : Log file not found! \033[0m\n");
            return NULL;
        }

        char **lines = NULL;
        size_t numLines = 0;
        size_t lineCapacity = 0;
        char *line = NULL;
        size_t len = 0;
        
        // Read the log file
        while (getline(&line, &len, logFile) != -1) {
            if (numLines >= lineCapacity) {
                lineCapacity = lineCapacity ? lineCapacity * 2 : 1;
                lines = realloc(lines, lineCapacity * sizeof(char*));
            }
            lines[numLines++] = strdup(line);
        }
        free(line);
        fclose(logFile);

        // In case the value-th line does not exist
        if ((size_t)value > numLines) {
            printf("\033[1;31m Oopsie Woopsie : Not enough commands in the log file! \033[0m\n");
            for (size_t i = 0; i < numLines; i++) {
                free(lines[i]);
            }
            free(lines);
            return "";
        }

        // Get the value-th command
        size_t targetIndex = numLines - value;
        strcpy(nxtCmnd, lines[targetIndex]);

        for (size_t i = 0; i < numLines; i++) {
            free(lines[i]);
        }
        free(lines);

        return nxtCmnd;
    }
    
    // In case of invalid parameters
    else {
        printf("\033[1;31m Oopsie Woopsie : Invalid log command! \033[0m\n");
        return "";
    }
}