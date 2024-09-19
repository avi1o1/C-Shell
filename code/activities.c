#include "activities.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/time.h>
#include <unistd.h>

// Function to print details of all processes spawned by the shell
int actities(char* HOME) {
    // Open the activities file
    char* filePath = (char*) malloc(256); 
    strcpy(filePath, HOME);
    strcat(filePath, "/systemFiles/activities.txt");

    FILE* file = fopen(filePath, "r");
    if (file == NULL) {
        printf("\033[1;31m Oopsie Woopsie: Could not open activities file.\033[0m\n");
        return 1;
    }

    // Open a temporary file to store updated process list
    char* tempFilePath = (char*) malloc(256); 
    strcpy(tempFilePath, HOME);
    strcat(tempFilePath, "/systemFiles/activities_temp.txt");

    FILE* tempFile = fopen(tempFilePath, "w");
    if (tempFile == NULL) {
        printf("\033[1;31m Oopsie Woopsie: Could not open temporary activities file.\033[0m\n");
        fclose(file);
        free(filePath);
        free(tempFilePath);
        return 1;
    }

    // Read the file line by line (process by process)
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, file)) != -1) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        // Extract the first word (PID) from the line
        char pid[32];
        sscanf(line, "%31s", pid);

        // Check the status of the process
        int status = getProcessStatus(pid);

        // If the process is not a zombie or does not have a missing /proc entry, keep it in the file
        if (status != -1) { // -1 indicates a zombie or missing /proc file
            fprintf(tempFile, "%s\n", line);
            // Print the status of the process
            if (status == 0) {
                printf("%s - \033[1;32mRunning\033[0m\n", line); // Running
            } else if (status == 1) {
                printf("%s - \033[1;31mStopped\033[0m\n", line); // Stopped
            }
        }
    }

    // Close the files and free the line buffer
    fclose(file);
    fclose(tempFile);
    if (line) {
        free(line);
    }

    // Replace the original activities file with the updated temporary file
    if (rename(tempFilePath, filePath) != 0) {
        printf("\033[1;31m Oopsie Woopsie: Could not update activities file.\033[0m\n");
        free(filePath);
        free(tempFilePath);
        return 1;
    }

    free(filePath);
    free(tempFilePath);
    return 0;
}

// Function to get the process status (Running, Stopped, or Zombie/Missing)
int getProcessStatus(const char* pid) {
    // Open the status file for the process
    char path[64];
    snprintf(path, sizeof(path), "/proc/%s/status", pid);
    FILE* statusFile = fopen(path, "r");

    if (statusFile == NULL) {
        // If process does not exist or is a zombie (i.e., /proc file is not found)
        return -1;
    }

    // Read the status file line by line
    char* statusLine = NULL;
    size_t statusLen = 0;
    ssize_t statusRead;
    while ((statusRead = getline(&statusLine, &statusLen, statusFile)) != -1) {
        if (strncmp(statusLine, "State:", 6) == 0) {
            // Extract the state of the process
            char state = statusLine[7];
            if (state == 'Z') { // Zombie process
                fclose(statusFile);
                if (statusLine) {
                    free(statusLine);
                }
                return -1;
            } else if (state == 'T') { // Stopped process
                fclose(statusFile);
                if (statusLine) {
                    free(statusLine);
                }
                return 1; // Stopped
            } else { // Running or other states
                fclose(statusFile);
                if (statusLine) {
                    free(statusLine);
                }
                return 0; // Running
            }
        }
    }

    // Close the file and free the line buffer
    fclose(statusFile);
    if (statusLine) {
        free(statusLine);
    }
    return -1;
}
