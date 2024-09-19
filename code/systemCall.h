#ifndef BASH_H
#define BASH_H

/*
    Function to execute a bash command
    @param cmnd: The command to be executed
    @return: 0 if the command was executed successfully, 1 otherwise
*/
int bashCommand(char* cmnd, char* HOME);

// Helper function to execute the command
int exeCuteCmnd(char *cmnd);

#endif