#ifndef ACT_H
#define ACT_H

/*
    Function to print details of all processes spawned by the shell
    @return 0 on success, 1 on failure
*/
int actities(char* HOME);

// Function to get the process status (Running, Stopped, or Zombie/Missing)
int getProcessStatus(const char* pid);

#endif