#ifndef REVEAL_H
#define REVEAL_H

#include <dirent.h>
#include <sys/stat.h>

/*
    Function to get the details of a directory
    @param a: The presence of the -a flag
    @param l: The presence of the -l flag
    @param path: Complete path of the directory to get the details of
    @return 0 on success, 1 on failure
*/
int reveal(int a, int l, char* path);

// Helper function to get the details of a directory (at the given path)
int getDirectoryDetails(char* path, int a, int l);

// Helper function to print the details of the given directory
int printDetails(const char* path, const struct dirent* entry);

// Function to convert file permissions to a string
void getPermissionsString(mode_t mode, char *str);

#endif