#include "systemCall.h"
#include "globals.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>

char* nxtCmndDisplay = NULL;

// Function to execute a bash command
int bashCommand(char* cmnd, char* HOME) {
    // Check if the command is to be run in the background
    int isBackground = 0;
    if (cmnd[strlen(cmnd) - 1] == '&') {
        isBackground = 1;
        cmnd[strlen(cmnd) - 2] = '\0';
    }
    
    // In case of a background process
    if (isBackground) {
        // Forking to run the command in the background
        int f = fork();
        if (f == 0) {
            setpgid(0, 0);

            char command[256];
            sscanf(cmnd, "%s", command);

            char check_cmd[512];
            snprintf(check_cmd, sizeof(check_cmd), "which %s > /dev/null 2>&1", command);

            // Check if the command exists
            if (exeCuteCmnd(check_cmd)) {
                return 1;
            }
            else {
                printf("\033[1;33m Background Process ID: %d\033[0m\n", getpid());
                // Log the command
                char* commandCopy = strdup(cmnd);

                char* aPath = (char*) malloc(256);
                strcpy(aPath, HOME);
                strcat(aPath, "/systemFiles/activities.txt");

                char* bPath = (char*) malloc(256);
                strcpy(bPath, HOME);
                strcat(bPath, "/systemFiles/bgOutput.txt");

                FILE* activities = fopen(aPath, "a");
                FILE* bgOutput = fopen(bPath, "a");
                fprintf(activities, "%d : %s\n", getpid(), commandCopy);
                fprintf(bgOutput, "%d : %s\n", getpid(), commandCopy);

                fclose(activities);
                fclose(bgOutput);

                // Execute the command
                exeCuteCmnd(cmnd);
            }
            exit(0);
        }
        else if (f > 0) {
            // Parent process of the background process
            setpgid(f, f);
            usleep(10000);
        }
        else {
            printf("\033[1;31m Oopsie Woopsie: Could not execute command! \033[0m\n");
            return 1;
        }
    }

    // In case of a foreground process
    else {
        struct timeval start, end;
        long seconds, useconds;
        double mtime;

        // Start the timer
        gettimeofday(&start, NULL);
        
        char command[256];
        sscanf(cmnd, "%s", command);
        char check_cmd[512];
        snprintf(check_cmd, sizeof(check_cmd), "which %s > /dev/null 2>&1", command);
        
        // Check if the command exists
        if (exeCuteCmnd(check_cmd)) {
            return 1;
        }
        else {
            exeCuteCmnd(cmnd);
        }

        // End the timer
        gettimeofday(&end, NULL);

        // Calculate the time taken
        seconds  = end.tv_sec  - start.tv_sec;
        useconds = end.tv_usec - start.tv_usec;
        mtime = (seconds*1000 + useconds/1000) + 0.5;

        // Print the time taken
        char processName[256];
        sscanf(cmnd, "%s", processName);
        printf("\033[1;33m %s returned in %f seconds\033[0m\n", processName, mtime / 1000);

        // Set the next command display, if the command took more than 2 seconds
        if (mtime > 2000) {
            char* cmndName = strtok(cmnd, " ");
            nxtCmndDisplay = (char*) malloc(strlen(cmndName) + 20);
            if (nxtCmndDisplay == NULL) {
                printf("\033[1;31m Oopsie Woopsie: Memory allocation failed! \033[0m\n");
                return 1;
            }
            sprintf(nxtCmndDisplay, " %s : %ds ", cmndName, (int)(mtime / 1000));
        }
    }
    return 0;
}

// Helper function to execute the command
int exeCuteCmnd(char *cmnd) {
    // Forking to execute the command
    pid_t pid = fork();

    // Child process executes the command
    if (pid == 0) {
        char *argv[] = {"sh", "-c", cmnd, NULL};
        execvp("sh", argv);

        // If execvp fails
        printf("\033[1;31m Oopsie Woopsie : Could not execute command! \033[0m\n");
        return 1;
    }

    // Parent process waits for the child process to finish
    else if (pid > 0) {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            printf("\033[1;31m Oopsie Woopsie: Could not execute command! \033[0m\n");
            return 1;
        }

        // Checking if the command was executed successfully
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            printf("\033[1;31m Oopsie Woopsie: Command not found! \033[0m\n");
            return 1;
        }
    }

    // Fork failed
    else {
        printf("\033[1;31m Oopsie Woopsie: Could not execute command! \033[0m\n");
        return 1;
    }
    
    return 0;
}
