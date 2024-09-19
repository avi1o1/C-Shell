#include "hop.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

// Function to change directory
int hop(char* path) {
    // Checking if the directory exists
    struct stat sb;
    if (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode)) {
        printf("%s\n", path);
    }
    else {
        printf("\033[1;31m Oopsie Woopsie : Directory not found \033[0m\n");
        return 1;
    }

    // Update current directory
    chdir(path);
    return 0;
}