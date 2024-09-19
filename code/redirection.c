#include "redirection.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>

#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>

// Helper function to trim leading and trailing whitespaces
char* removeWhitespace(char* str) {
    // Trim leading whitespaces
    while (isspace(*str)) {
        str++;
    }

    // Check for empty string
    if (*str == 0) {
        return str;
    }

    // Trim trailing whitespaces
    char* end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) {
        end--;
    }

    // Write new null terminator
    end[1] = '\0';
    return str;
}

// Helper function to read the contents of a file into a string
char* readFileContent(char* filename) {
    // Open the file
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("\033[1;31m Oopsie Woopsie : Error opening file. \033[0m\n");
        return NULL;
    }

    // Get the length of the file
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Read the contents of the file
    char* content = (char*) malloc(length+1);
    if (!content) {
        printf("\033[1;31m Oopsie Woopsie : Memory allocation failed. \033[0m\n");
        return NULL;
    }
    fread(content, 1, length, file);
    content[length] = '\0';

    // Close the file and return the content
    fclose(file);
    return content;
}

// Helper function to get wc of a file
char* wcOutput(char* data) {
    // Setting up variables
    int lines = 1, words = 0, characters = 0;
    int inWord = 0;

    // Counting the number of lines, words and characters
    for (int i = 0; data[i]; i++) {
        characters++;
        if (data[i] == '\n') {
            lines++;
        }
        if (isspace(data[i])) {
            inWord = 0;
        }
        else if (!inWord) {
            inWord = 1;
            words++;
        }
    }

    // Creating the output string
    char* output = (char*) malloc(64);
    if (output == NULL) {
        printf("\033[1;31m Oopsie Woopsie : Memory allocation failed. \033[0m\n");
        return NULL;
    }
    sprintf(output, "Lines = %d ; Words = %d ; Characters = %d", lines, words, characters);
    return output;
}

// Helper function to write to a file in specified mode
int writeToFile(int writeMode, char* output, char* outputFile) {
    // If the command has to be written to a file
    if (writeMode) {
        // Trim leading and trailing whitespaces
        outputFile = removeWhitespace(outputFile);

        // Open the file in appropriate mode
        FILE* outFile = fopen(outputFile, writeMode == 1 ? "a" : "w");
        if (outFile == NULL) {
            printf("\033[1;31m Oopsie Woopsie : Error opening file. \033[0m\n");
            return 1;
        }

        // Write the output to the file
        fprintf(outFile, "%s\n", output);
        fclose(outFile);
    }

    // Otherwise, print to stdout
    else {
        printf("%s\n", output);
    }

    return 0;
}

// Function to redirect input/output of a command
int redirection(char* tokens) {
    // Seperating the main command
    char* cmd = strdup(tokens);
    tokens = strtok(NULL, " \t");

    // Obtaining the complete input string
    char* cmnd = (char*) malloc(256);
    if (cmnd == NULL) {
        printf("\033[1;31m Oopsie Woopsie : Memory allocation failed.\033[0m\n");
        return 1;
    }
    memset(cmnd, 0, 256);
    while (tokens) {
        strcat(cmnd, tokens);
        strcat(cmnd, " ");
        tokens = strtok(NULL, " ");
    }

    // Checking if the command has to be written to a file
    int writeMode = 0;
    char* outputFile = (char*) malloc(64);
    char* write = strstr(cmnd, ">>");
    if (write != NULL) {
        // Append mode
        writeMode = 1;
        strcpy(outputFile, write + 3);
        cmnd[strlen(cmnd) - strlen(outputFile) - 3] = '\0';
    }
    else {
        write = strstr(cmnd, ">");
        if (write != NULL) {
            // Write mode
            writeMode = 2;
            strcpy(outputFile, write + 2);
            cmnd[strlen(cmnd) - strlen(outputFile) - 2] = '\0';
        }
    }

    // Checking if the command has to be read from a file
    char* pipers = (char*) malloc(256);
    pipers[0] = '\0';
    char* readMode = strstr(cmnd, "<");
    if (readMode != NULL) {
        strcpy(pipers, readMode + 2);
        cmnd[strlen(cmnd) - strlen(pipers) - 2] = '\0';
    }

    // Setting up variables
    char* data = NULL;
    if (pipers[0] == '\0') {
        pipers = strdup(cmnd);
    }
    pipers = removeWhitespace(pipers);
    outputFile = removeWhitespace(outputFile);

    // Handling piped commands
    char* ind = strtok(pipers, "|");
    while (ind) {
        // Taking one pipe at a time
        pipers = removeWhitespace(ind);

        // wc encountered
        if (!strcmp(pipers, "wc")) {
            data = wcOutput(data);
        }

        // cat encountered
        else if (!strcmp(pipers, "cat")) {
            ind = strtok(NULL, "|");
            continue;
        }

        // quotes (text) encountered
        else if (pipers[0] == '\'' || pipers[0] == '\"') {
            data = strdup(pipers + 1);
            data[strlen(data) - 1] = '\0';
            data = removeWhitespace(data);
        }

        // sed encountered
        else if (!strcmp(pipers, "sed 's/ //g'")) {
            char* temp = (char*) malloc(strlen(data) + 1);
            if (temp == NULL) {
                printf("\033[1;31m Oopsie Woopsie : Memory allocation failed. \033[0m\n");
                return 1;
            }
            int i = 0, j = 0;
            while (data[i]) {
                if (data[i] != ' ') {
                    temp[j++] = data[i];
                }
                i++;
            }
            temp[j] = '\0';
            data = temp;
        }

        // Invalid piping
        else if (pipers[0] == '\0') {
            printf("\033[1;31m Oopsie Woopsie : Invalid piping. \033[0m\n");
            free(cmd);
            free(cmnd);
            return 1;
        }

        // otherwise, deal it like a filename
        else {
            data = readFileContent(pipers);
            if (data == NULL) {
                return 1;
            }
        }

        ind = strtok(NULL, "|");
    }

    if (!strcmp(cmd, "wc")) {
        data = wcOutput(data);
    }
    
    else if ((strcmp(cmd, "echo") && strcmp(cmd, "cat"))) {
        printf("\033[1;31m Oopsie Woopsie : Command not found. \033[0m\n");
        free(cmd);
        free(cmnd);
        return 1;
    }

    // Writing to file or stdout
    if (!data) {
        printf("\033[1;31m Oopsie Woopsie : Missing data encountered. \033[0m\n");
        free(cmd);
        free(cmnd);
        return 1;
    }
    if (writeToFile(writeMode, data, outputFile)) {
        return 1;
    }

    // Free the allocated memory
    free(cmd);
    free(cmnd);
    return 0;
}
