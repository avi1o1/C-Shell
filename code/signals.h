#ifndef SIGNAL_H
#define SIGNAL_H

/*
    Function to send signal to a process
    @param cmnd: The command to be executed
    @return: 0 if the command was executed successfully, 1 otherwise
*/
int ping(int pid, int signal);

#endif