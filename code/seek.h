#ifndef SEEK_H
#define SEEK_H

#include <dirent.h>

/*
    Function to seek a file/dir in the given directory (or CWD, if not specified)
    @param name: Name of the file/dir to seek
    @param d: The presence of the -d flag
    @param e: The presence of the -e flag
    @param f: The presence of the -f flag
    @param loc: The location to seek in
    @return 0 on success, 1 on failure
*/
int seek(char* name, int d, int e, int f, char* loc);

// Helper function to print the contents of a directory
int printDirDetails(char* path);

// Helper function to print the contents of a file
int printFileDetails(char* path);

#endif