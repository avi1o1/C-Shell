#include "neonate.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <termios.h>
#include <ctype.h>
#include <time.h>

#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/wait.h>

// Function to print the most recently created process
int neonate(int time) {
    struct termios originalSettings;
    setNonCanonicalMode(&originalSettings);

    while (1) {
        // Record the start time
        struct timespec start, current;
        clock_gettime(CLOCK_MONOTONIC, &start);

        while (1) {
            // Set up the file descriptor set
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(STDIN_FILENO, &readfds);

            // Set up a timeout checker (100 milliseconds)
            struct timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = 100000;

            // Wait for input or timeout
            int retval = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);

            if (retval == -1) {
                printf("\033[0;31m Oopsie Woopsie : select() failed. \033[0m\n");
                restoreTerminalMode(&originalSettings);
                return 1;
            }
            
            else if (retval) {
                // Data is available to read
                char c = getchar();
                if (c == 'x' || c == 'X') {
                    restoreTerminalMode(&originalSettings);
                    return 0;
                }
            }

            // Check if the specified interval has passed
            clock_gettime(CLOCK_MONOTONIC, &current);
            if ((current.tv_sec - start.tv_sec) >= time) {
                break;
            }
        }

        // Timeout occurred, print the most recently created process
        int N = getMostRecentPID();
        if (N == -1) {
            restoreTerminalMode(&originalSettings);
            return 1;
        }
        else {
            printf("%d\n", N);
        }
    }

    // Restore terminal settings before returning
    restoreTerminalMode(&originalSettings);
    return 0;
}

// Function to get the PID of the most recently created process
int getMostRecentPID() {
    DIR *dir;
    struct dirent *entry;
    int max_pid = -1;

    // Open the /proc directory
    dir = opendir("/proc");
    if (dir == NULL) {
        printf("\033[0;31m Oopsie Woopsie : Couldn't open the /proc directory\033[0m\n");
        return -1;
    }

    // Find the most recent (maximum) PID
    while ((entry = readdir(dir)) != NULL) {
        if (isNumeric(entry->d_name)) {
            int pid = atoi(entry->d_name);
            if (pid > max_pid) {
                max_pid = pid;
            }
        }
    }

    // Close the /proc directory
    closedir(dir);
    return max_pid;
}

// Function to check if a string consists only of digits
int isNumeric(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) {
            return 0;
        }
    }
    return 1;
}

// Function to set terminal to non-canonical mode
void setNonCanonicalMode(struct termios* original) {
    struct termios newSettings;

    // Get current terminal settings
    tcgetattr(STDIN_FILENO, original);
    newSettings = *original;

    // Disable canonical mode and echo
    newSettings.c_lflag &= ~(ICANON | ECHO);

    // Apply new settings
    tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);
}

// Function to restore terminal to original settings
void restoreTerminalMode(struct termios* original) {
    tcsetattr(STDIN_FILENO, TCSANOW, original);
}
