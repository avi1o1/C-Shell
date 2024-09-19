#ifndef NEONATE_H
#define NEONATE_H

#include <termios.h>

/*
    Function to print the most recently created process
    @param tokens: Process ID
    @return 0 on success, 1 on failure
*/
int neonate(int time);

// Function to set terminal to non-canonical mode
void setNonCanonicalMode(struct termios *original);

// Function to restore terminal to original settings
void restoreTerminalMode(struct termios *original);

// Helper function to check if file name is numeric
int isNumeric(const char *str);

// Helper function to get the most recent PID
int getMostRecentPID();

#endif