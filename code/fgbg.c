#include "fgbg.h"
#include "globals.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <fcntl.h>

#include <sys/time.h>
#include <sys/wait.h>

// Function to bring a background process to the foreground
int fg(int pid) {
    // Check if process exists
    char proc_path[256];
    snprintf(proc_path, sizeof(proc_path), "/proc/%d", pid);
    if (access(proc_path, F_OK)) {
        printf("\033[1;31m No process with PID %d found\033[0m\n", pid);
        return 1;
    }

    // Fetching process details
    char stat_path[256];
    snprintf(stat_path, sizeof(stat_path), "/proc/%d/stat", pid);
    FILE* stat_file = fopen(stat_path, "r");
    if (stat_file == NULL) {
        printf("\033[1;31m Error opening stat file for PID %d.\033[0m\n", pid);
        return 1;
    }

    int pgrp;
    char state;
    fscanf(stat_file, "%*d %*s %c %*d %d", &state, &pgrp);
    fclose(stat_file);

    // Check if the process is a background process
    int fg_pgrp = tcgetpgrp(STDIN_FILENO);
    if (pgrp == fg_pgrp) {
        printf("\033[1;31m Process with PID %d is already in the foreground.\033[0m\n", pid);
        return 1;
    }

    // Ignore SIGTTOU, SIGTTIN, and SIGTSTP signals temporarily
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);

    // Change the terminal's foreground process group
    if (tcsetpgrp(STDIN_FILENO, pgrp) == -1) {
        printf("\033[1;31m Oopsie Woopsie : Error setting foreground process group.\033[0m\n");
        return 1;
    }

    // Start the timer
    struct timeval start, end;
    long seconds, useconds;
    double mtime;
    gettimeofday(&start, NULL);

    // Resuming the process if it is stopped
    if (state == 'T') {
        if (kill(pid, SIGCONT) == -1) {
            printf("\033[1;31m Oopsie Woopsie : Error resuming process with PID %d.\033[0m\n", pid);
            return 1;
        }
    }

    // Wait for the process to finish
    int status;
    if (waitpid(pid, &status, WUNTRACED) == -1) {
        printf("\033[1;31m Oopsie Woopsie : Error waiting for process with PID %d.\033[0m\n", pid);
        return 1;
    }

    // Restore the original foreground process group
    if (tcsetpgrp(STDIN_FILENO, fg_pgrp) == -1) {
        printf("\033[1;31m Oopsie Woopsie : Error restoring foreground process group.\033[0m\n");
        return 1;
    }

    // End the timer
    gettimeofday(&end, NULL);

    // Calculate the time taken
    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    mtime = (seconds*1000 + useconds/1000) + 0.5;

    // Set the next command display, if the command took more than 2 seconds
    printf("\033[1;33m fg returned in %f seconds\033[0m\n", mtime / 1000);
    if (mtime > 2000) {
        nxtCmndDisplay = (char*) malloc(24);
        if (nxtCmndDisplay == NULL) {
            printf("\033[1;31m Oopsie Woopsie: Memory allocation failed! \033[0m\n");
            return 1;
        }
        sprintf(nxtCmndDisplay, " fg : %ds ", (int)(mtime / 1000));
    }

    // Restore the default signal handlers
    signal(SIGTTOU, SIG_DFL);
    signal(SIGTTIN, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);

    return 0;
}

// Function to change a stopped background process to running
int bg(int pid) {
    // Check if process exists
    char proc_path[256];
    snprintf(proc_path, sizeof(proc_path), "/proc/%d", pid);
    if (access(proc_path, F_OK)) {
        printf("\033[1;31m No process with PID %d found.\033[0m\n", pid);
        return 1;
    }

    // Fetching process details
    char stat_path[256];
    snprintf(stat_path, sizeof(stat_path), "/proc/%d/stat", pid);
    FILE* stat_file = fopen(stat_path, "r");
    if (stat_file == NULL) {
        printf("\033[1;31m Error opening stat file for PID %d.\033[0m\n", pid);
        return 1;
    }

    int pgrp;
    char state;
    fscanf(stat_file, "%*d %*s %c %*d %d %*d %*d %*d", &state, &pgrp);
    fclose(stat_file);

    // Check if the process is a background process
    if (pid == getpid()) {
        printf("\033[1;31m Process with PID %d is not a background process.\033[0m\n", pid);
        return 1;
    }

    // Check if the process is stopped
    if (state != 'T') {
        printf("\033[1;31m Process with PID %d is not stopped.\033[0m\n", pid);
        return 1;
    }

    // Continue the process
    kill(pid, 18);
    
    printf("\033[1;32m Process with PID %d continued.\033[0m\n", pid);
    return 0;
}
