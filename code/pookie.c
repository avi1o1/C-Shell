#include "pookie.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Function to get the details of a process
int pookieLore(char* tokens) {
    // Open the status file of the process
    char path[128];
    if (!tokens) {
        sprintf(path, "/proc/%d/status", getpid());
    }
    else {
        if (atoi(tokens) <= 0) {
            printf("\033[1;31m Oopsie Woopsie : Invalid PID \033[0m\n");
            return 1;
        }
        sprintf(path, "/proc/%s/status", tokens);
    }

    FILE* file = fopen(path, "r");
    if (file == NULL) {
        printf("\033[1;31m Oopsie Woopsie : Invalid PID \033[0m\n");
        return 1;
    }
    
    // Get details of the process
    char line[128];
    char pid[32] = "";
    char state[32] = "";
    char gid[32] = "";
    char vmsize[32] = "";
    char name[128] = "";
    char exePath[256] = "";

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "Pid:", 4) == 0) {
            strncpy(pid, line + 5, sizeof(pid) - 1);
        } else if (strncmp(line, "State:", 6) == 0) {
            strncpy(state, line + 7, sizeof(state) - 1);
        } else if (strncmp(line, "Tgid:", 5) == 0) {
            strncpy(gid, line + 6, sizeof(gid) - 1);
        } else if (strncmp(line, "VmSize:", 7) == 0) {
            strncpy(vmsize, line + 8, sizeof(vmsize) - 1);
        } else if (strncmp(line, "Name:", 5) == 0) {
            strncpy(name, line + 6, sizeof(name) - 1);
        }
    }

    // Determine if the process is in the foreground or background
    int pgidInt = atoi(gid);
    if (pgidInt == getpid()) {
        state[1] = '+';
        state[2] = '\n';
        state[3] = '\0';
    } else {
        state[1] = '\n';
        state[2] = '\0';
    }
    
    // Check if the data is empty
    if (pid[0] == '\0') {
        strcpy(pid, "No Data Found\n");
    }
    if (gid[0] == '\0') {
        strcpy(gid, "No Data Found\n");
    }
    if (state[0] == '\0') {
        strcpy(state, "No Data Found\n");
    }
    if (name[0] == '\0') {
        strcpy(name, "No Data Found\n");
    }
    if (vmsize[0] == '\0') {
        strcpy(vmsize, "No Data Found\n");
    }

    // Get the full executable path
    if (!tokens) {
        sprintf(path, "/proc/%d/exe", getpid());
    } else {
        sprintf(path, "/proc/%s/exe", tokens);
    }
    ssize_t len = readlink(path, exePath, sizeof(exePath) - 1);
    if (len != -1) {
        exePath[len] = '\0';
    } else {
        strcpy(exePath, "No Data Found");
    }

    // Print the stored information
    printf("Process ID : %s", pid);
    printf("Group ID : %s", gid);
    printf("Process Status : %s", state);
    printf("Executable Path : %s\n", exePath);
    printf("Virtual Memory : %s", vmsize);
    
    fclose(file);
    return 0;
}