#include "reveal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pwd.h>

#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

// Function to get the details of a directory
int reveal(int a, int l, char* path) {
    // Check if the path is valid (exists and is a directory)
    struct stat sb;
    if (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode)) {
        int tmp = getDirectoryDetails(path, a, l);
        if (tmp == 1) {
            return 1;
        }
    }
    else {
        printf("\033[1;31m Oopsie Woopsie : Directory not found \033[0m\n");
        return 1;
    }
    return 0;
}

// Helper function to get the details of a directory (at the given path)
int getDirectoryDetails(char* path, int a, int l) {
    // Open the directory
    DIR* dir = opendir(path);
    if (dir == NULL) {
        printf("\033[1;31m Oopsie Woopsie : We encountered some error \033[0m\n");
        return 1;
    }

    // Read the directory
    long totalBlocks = 0;
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (a || entry->d_name[0] != '.') {
            if (l) {
                // Print details if -l flag is set
                int tmp = printDetails(path, entry);
                if (tmp == -1) {
                    return 1;
                }
                totalBlocks += tmp;
            }

            else {
                struct stat fileStat;
                char fullPath[1024];
                snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name);

                // Get file details
                if (stat(fullPath, &fileStat) == -1) {
                    printf("\033[1;31m Oopsie Woopsie : We encountered some error \033[0m\n");
                    return 1;
                }

                // Print file name with color coding
                if (S_ISDIR(fileStat.st_mode)) {
                    // Directory
                    printf("\033[1;34m%s\033[0m\n", entry->d_name);
                } else if (fileStat.st_mode & S_IXUSR) {
                    // Executable
                    printf("\033[1;32m%s\033[0m\n", entry->d_name);
                } else {
                    // Regular file
                    printf("%s\n", entry->d_name);
                }
            }
        }
    }

    // Print total blocks used if -l flag is set
    if (l) {
        printf("Total blocks [of 1MB block size] : %ld\n", totalBlocks/2);
    }

    // Close the directory and return
    closedir(dir);
    return 0;
}

// Helper function to print the details of the given directory (and return the number of blocks used)
int printDetails(const char* path, const struct dirent* entry) {
    struct stat fileStat;
    char fullPath[1024];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name);

    // Check if the file exists
    if (stat(fullPath, &fileStat) == -1) {
        printf("\033[1;31m Oopsie Woopsie : We encountered some error \033[0m\n");
        return -1;
    }

    // Get file details
    char permissions[11];
    getPermissionsString(fileStat.st_mode, permissions);
    struct passwd *pw = getpwuid(fileStat.st_uid);
    const char *owner = (pw != NULL) ? pw->pw_name : "Unknown";

    // Print file name with color coding
    if (S_ISDIR(fileStat.st_mode)) {
        // Directory
        printf("\033[1;34m %s\033[0m\n", entry->d_name);
    } else if (fileStat.st_mode & S_IXUSR) {
        // Executable
        printf("\033[1;32m%s\033[0m\n", entry->d_name);
    } else {
        // Regular file
        printf("%s\n", entry->d_name);
    }

    // Print other file details
    printf("\t Size: %ld bytes\n", fileStat.st_size);
    printf("\t Owner: %s\n", owner);
    printf("\t Permissions: -%s\n", permissions);
    printf("\t Last modified: %s\n", ctime(&fileStat.st_mtime));
    
    return fileStat.st_blocks;
}

// Helper function to convert file permissions to a string
void getPermissionsString(mode_t mode, char* str) {
    str[0] = (mode & S_IRUSR) ? 'r' : '-';
    str[1] = (mode & S_IWUSR) ? 'w' : '-';
    str[2] = (mode & S_IXUSR) ? 'x' : '-';
    str[3] = (mode & S_IRGRP) ? 'r' : '-';
    str[4] = (mode & S_IWGRP) ? 'w' : '-';
    str[5] = (mode & S_IXGRP) ? 'x' : '-';
    str[6] = (mode & S_IROTH) ? 'r' : '-';
    str[7] = (mode & S_IWOTH) ? 'w' : '-';
    str[8] = (mode & S_IXOTH) ? 'x' : '-';
    str[9] = '\0';
}
