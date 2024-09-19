#ifndef FGBG_H
#define FGBG_H

/*
    Function to bring a background process to the foreground
    @param pid: Process ID of the process to bring to foreground
    @return 0 on success, 1 on failure
*/
int fg(int pid);

/*
    Function to change a stopped background process to running
    @param pid: Process ID of the process to execute
    @return 0 on success, 1 on failure
*/
int bg(int pid);

#endif