#include "logging.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// Logs the command to the log file
void logToFile(char* cmnd, char* HOME) {
    // Getting the path of the log file
    size_t logFilePathLen = strlen(HOME) + strlen("/systemFiles/cmndlogs.txt") + 1;
    char* logFilePath = (char*)malloc(logFilePathLen);
    if (logFilePath == NULL) {
        printf("\033[1;31m Oopsie Woopsie : Memory error detected! \033[0m\n");
        return;
    }
    strcpy(logFilePath, HOME);
    strcat(logFilePath, "/systemFiles/cmndlogs.txt");

    FILE *logFile = fopen(logFilePath, "r");
    if (logFile == NULL) {
        printf("\033[1;31m Oopsie Woopsie : Couldn't log the command (Log file not found!) \033[0m\n");
        free(logFilePath);
        return;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char *commands[15];
    int count = 0;

    // Read all lines into the commands array
    while ((read = getline(&line, &len, logFile)) != -1) {
        // Remove newline character from line if it exists
        if (line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }

        // Make sure the array doesn't exceed 15 commands
        if (count == 15) {
            free(commands[0]);
            for (int i = 1; i < 15; i++) {
                commands[i - 1] = commands[i];
            }
            count--;
        }

        // Store the new command
        commands[count] = strdup(line);
        if (commands[count] == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            break;
        }
        count++;
    }
    fclose(logFile);
    free(line);

    // Check if the last command is different from the new command
    if (count == 0 || strcmp(commands[count - 1], cmnd) != 0) {
        // Replace the last command with the new command
        if (count == 15) {
            free(commands[0]);
            for (int i = 1; i < 15; i++) {
                commands[i - 1] = commands[i];
            }
            count--;
        }
        commands[count] = strdup(cmnd);
        if (commands[count] == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            for (int i = 0; i < count; i++) {
                free(commands[i]);
            }
            free(logFilePath);
            return;
        }
        count++;
    }

    // Write the commands back to the file
    logFile = fopen(logFilePath, "w");
    if (logFile == NULL) {
        printf("\033[1;31m Oopsie Woopsie : Couldn't log the command! \033[0m\n");
        for (int i = 0; i < count; i++) {
            free(commands[i]);
        }
        free(logFilePath);
        return;
    }

    // Write commands to the file and free memory
    for (int i = 0; i < count; i++) {
        fprintf(logFile, "%s\n", commands[i]);
        free(commands[i]);
    }
    fclose(logFile);
    free(logFilePath);
}
