#include "seek.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#include <time.h>
#include <sys/stat.h>

// Function to seek a file/dir in the given directory
int seek(char* name, int d, int e, int f, char* loc) {
    int found = 0;

    // Open the loc directory
    DIR* dir = opendir(loc);
    if (!dir) {
        printf("\033[1;31m Oopsie Woopsie: Could not open directory %s.\033[0m\n", loc);
        return 0;
    }

    // Check all files and directories in the directory
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        // If the entry is a directory
        if (entry->d_type == DT_DIR) {
            // Skip the current and parent directories
            if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
                continue;
            }

            // If -e flag is present and the name matches, print the contents of the directory
            if (e && strcmp(entry->d_name, name) == 0) {
                // Get full path of the directory
                size_t path_len = strlen(loc) + strlen(entry->d_name) + 2;
                char* path = (char*)malloc(path_len);
                if (!path) {
                    printf("\033[1;31m Oopsie Woopsie: Memory allocation failed.\033[0m\n");
                    closedir(dir);
                    return -1;
                }
                snprintf(path, path_len, "%s/%s", loc, entry->d_name);

                // Print the contents of the directory
                int x = printDirDetails(path);
                if (x == -1) {
                    free(path);
                    closedir(dir);
                    return -1;
                }

                free(path);
                return 1;
            }

            // If -e flag is present and a directory is already found
            if (e && found) {
                continue;
            }

            // If -f flag is absent and the name matches, print the directory
            if (!f && strcmp(entry->d_name, name) == 0) {
                printf("\033[1;34m%s/%s\033[0m\n", loc, entry->d_name);
                found++;
            }

            // Recursively search the directory
            size_t path_len = strlen(loc) + strlen(entry->d_name) + 2;
            char* path = (char*)malloc(path_len);
            if (!path) {
                printf("\033[1;31m Oopsie Woopsie: Memory allocation failed.\033[0m\n");
                closedir(dir);
                return -1;
            }
            snprintf(path, path_len, "%s/%s", loc, entry->d_name);
            int x = seek(name, d, e, f, path);
            free(path);
            if (x == -1) {
                closedir(dir);
                return -1;
            }
            else {
                found += x;
            }
        }

        // If the entry is a file
        else if (entry->d_type == DT_REG) {
            // If -d flag is present, skip the file
            if (d) {
                // -d flag present
                continue;
            }
            
            // Get file name without extension
            char* baseName = strdup(entry->d_name);
            if (!baseName) {
                printf("\033[1;31m Oopsie Woopsie: Memory allocation failed.\033[0m\n");
                closedir(dir);
                return -1;
            }
            char* dot = strrchr(baseName, '.');
            if (dot != NULL) {
                *dot = '\0';
            }

            // If file name does not match
            if (strcmp(baseName, name) && strcmp(entry->d_name, name)) {
                free(baseName);
                continue;
            }

            // If -e flag is present, print the contents of the file
            if (e) {
                // Get full path of the file
                size_t path_len = strlen(loc) + strlen(entry->d_name) + 2;
                char* path = (char*)malloc(path_len);
                if (!path) {
                    printf("\033[1;31m Oopsie Woopsie: Memory allocation failed.\033[0m\n");
                    free(baseName);
                    closedir(dir);
                    return -1;
                }
                snprintf(path, path_len, "%s/%s", loc, entry->d_name);

                // Print the contents of the file
                int x = printFileDetails(path);
                if (x == -1) {
                    free(path);
                    free(baseName);
                    closedir(dir);
                    return -1;
                }

                free(path);
                free(baseName);
                closedir(dir);
                return 1;
            }

            // Otherwise, print the file
            printf("\033[1;32m%s/%s\033[0m\n", loc, entry->d_name);
            found++;
            free(baseName);
        }
    }

    // Close the directory
    closedir(dir);
    return found;
}

// Helper function to print the contents of a directory
int printDirDetails(char* path) {
    // Open the directory
    DIR* subdir = opendir(path);
    if (!subdir) {
        printf("\033[1;31m Oopsie Woopsie: Missing permission for task.\033[0m\n");
        return -1;
    }

    // Print the contents of the directory
    struct dirent* subentry;
    while ((subentry = readdir(subdir)) != NULL) {
        if (subentry->d_type == DT_DIR) {
            // Print directories in blue
            printf("\033[1;34m%s/%s\033[0m\n", path, subentry->d_name);
        }
        else {
            // Print files in green
            printf("\033[1;32m%s/%s\033[0m\n", path, subentry->d_name);
        }
    }

    // Close the directory
    closedir(subdir);
    return 1;
}

// Helper function to print the contents of a file
int printFileDetails(char* path) {
    FILE* file = fopen(path, "r");
    if (!file) {
        printf("\033[1;31m Oopsie Woopsie: Missing permission for task.\033[0m\n");
        return -1;
    }
                
    char c;
    while ((c = fgetc(file)) != EOF) {
        printf("%c", c);
    }
    printf("\n");

    fclose(file);
    return 1;
}
