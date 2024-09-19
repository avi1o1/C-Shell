#include "signals.h"

#include <stdio.h>
#include <signal.h>
#include <unistd.h>

// Function to send signal to a process
int ping(int pid, int signal) {
    // Check if process exists
    char procPath[256];
    sprintf(procPath, "/proc/%d", pid);
    if (access(procPath, 0) == -1) {
        printf("\033[1;31m Process %d does not exist\033[0m\n", pid);
        return 1;
    }

    // Ping the process
    signal %= 32;
    if (kill(pid, signal) == -1) {
        printf("\033[1;31m Oopsie Woopsie : Error sending signal %d to process %d\033[0m\n", signal, pid);
        return 1;
    }

    // Confirmation and return
    printf("\033[1;33m Signal %d sent to process %d\033[0m\n", signal, pid);
    return 0;
}