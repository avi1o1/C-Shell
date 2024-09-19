#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>

#include <sys/utsname.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>

#include "globals.h"
#include "hop.h"
#include "reveal.h"
#include "log.h"
#include "logging.h"
#include "systemCall.h"
#include "pookie.h"
#include "seek.h"
#include "myshrc.h"
#include "redirection.h"
#include "activities.h"
#include "signals.h"
#include "fgbg.h"
#include "neonate.h"
#include "manual.h"

char HOME[128];
char cwd[128];
char prev[128];

// Remove leading and trailing whitespace
char* trim(char* str) {
    char* end;
    while (isspace((unsigned char)*str)) {
        str++;
    }
    if (*str == 0) {
        return str;
    }
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }
    end[1] = '\0';
    return str;
}

// Spec 0 : Command Execution
int callCommand(char* tokens);

// Spec 1 : Display Requirement
void prompt() {
    char *userName = getlogin();                                                    // From the unistd.h library

    struct utsname unameData;                                                       // From the utsname.h library
    uname(&unameData);
    char *systemName = unameData.nodename;

    char* userInfo = (char*)malloc(256);
    if (strncmp(cwd, HOME, strlen(HOME)) == 0) {
        sprintf(userInfo, "\033[34m%s@%s\033[0m:\033[32m~%s\033[0m", userName, systemName, cwd + strlen(HOME));
    }
    else {
        sprintf(userInfo, "\033[34m%s\033[0m@\033[34m%s\033[0m:\033[32m%s\033[0m", userName, systemName, cwd);
    }

    if (nxtCmndDisplay) {
        printf("<%s%s> ", userInfo, nxtCmndDisplay);
        nxtCmndDisplay = "";
    }
    else {
        printf("<%s> ", userInfo);
    }
    free(userInfo);
}

// Getting the Absolute File Path
char* path(char* fileName) {
    char *absPath = (char*)malloc(256);

    if (fileName == NULL) {
        strcpy(absPath, cwd);
    }

    else if (fileName[0] == '~') {
        // Absolute Path from Home Directory
        strcpy(absPath, HOME);
        strcat(absPath, fileName + 1);
    }

    else if (fileName[0] == '/') {
        // Absolute Path from Root Directory
        strcpy(absPath, fileName);
    }

    else if (!strcmp(fileName, "..")) {
        // Parent Directory
        strcpy(absPath, cwd);
        int i = strlen(absPath) - 1;
        while (i >= 0 && absPath[i] != '/') {
            i--;
        }
        if (i > 0) {
            absPath[i] = '\0';
        }
        else {
            absPath[i+1] = '\0';
        }
    }

    else if (!strcmp(fileName, ".")) {
        // Current Directory
        strcpy(absPath, cwd);
    }

    else if (!strcmp(fileName, "~")) {
        // Home Directory
        strcpy(absPath, HOME);
    }

    else if (!strcmp(fileName, "-")) {
        // Previous Directory
        strcpy(absPath, prev);
    }

    else {
        // Relative Path
        strcpy(absPath, cwd);
        if (absPath[strlen(absPath) - 1] != '/') {
            strcat(absPath, "/");
        }
        strcat(absPath, fileName);
    }
    
    return absPath;
}

// Calling the hop function
int callHop(char* tokens) {
    // If not path is provided (hops to .)
    if (!tokens) {
        printf("%s\n", cwd);
    }

    // In case of multiple paths
    int curr = 0;
    while (tokens) {
        // printf("Path: %s\n", tokens);
        char* fullPath = path(tokens);
        curr = hop(fullPath);
        if (curr == 1) {
            return 1;
        }
        
        strcpy(prev, cwd);
        strcpy(cwd, fullPath);
        tokens = strtok(NULL, " \t");
    }
    return 0;
}

// Spec 5 : Log function
void callLog(char* tokens) {
    // Getting the path of the log file
    size_t logFilePathLen = strlen(HOME) + strlen("/systemFiles/cmndlogs.txt") + 1;
    char* logFilePath = (char*) malloc(logFilePathLen);
    if (logFilePath == NULL) {
        printf("\033[1;31m Oopsie Woopsie : Memory error detected! \033[0m\n");
        return;
    }
    strcpy(logFilePath, HOME);
    strcat(logFilePath, "/systemFiles/cmndlogs.txt");

    char nxtCmnd[1024];
    // memset(nxtCmnd, 0, sizeof(nxtCmnd));
    strcpy(nxtCmnd, notTheMathLog(tokens, logFilePath));

    if (nxtCmnd[0] != '\0') {
        size_t len = strlen(nxtCmnd);
        if (len > 0 && nxtCmnd[len - 1] == '\n') {
            nxtCmnd[len - 1] = '\0';
        }
        printf("\033[1;33m Executing: %s \033[0m\n", nxtCmnd);
        callCommand(nxtCmnd);
    }
    return;
}

// Spec 8 : Seek function
int seekHelp(char* tokens) {
    int d, f, e;
    d = f = e = 0;
    while (tokens && tokens[0] == '-') {
        if (strchr(tokens, 'd')) {
            d = 1;
        }
        if (strchr(tokens, 'e')) {
            e = 1;
        }
        if (strchr(tokens, 'f')) {
            f = 1;
        }
        tokens = strtok(NULL, " \t");
    }

    if (d == 1 && f == 1) {
        printf("\033[1;31m Oopsie Woopsie : Both -d and -f flags cannot be used together! \033[0m\n");
        return 1;
    }

    if (!tokens) {
        printf("\033[1;31m Oopsie Woopsie : No file name provided! \033[0m\n");
        return 1;
    }
    char* name = strdup(tokens);
    char* loc = strtok(NULL, " \t");
    if (!loc) {
        loc = ".";
    }

    int ret = seek(name, d, e, f, loc);
    if (ret == -1) {
        return 1;
    }
    else if (ret == 0) {
        printf("\033[1;33m Oopsie Woopsie : No match found! \033[0m\n");
    }
    return 0;
}

// Command Execution
int callCommand(char* cmnd) {
    char* cmndCopy = strdup(cmnd);
    int isInvalidCommand = 1;

    // Removing the newline character from the command and tokenizing it
    size_t len = strlen(cmnd);
    if (len > 0 && cmnd[len - 1] == '\n') {
        cmnd[len - 1] = '\0';
    }

    // Check for output redirection ">" or ">>"
    char* outputRedir = strstr(cmnd, ">");
    int fd, saved_stdout;

    if (outputRedir) {
        // Determine if it's ">>" (append) or ">" (overwrite)
        int append = (outputRedir[1] == '>');

        // Extract the command before ">" and the file after it
        *outputRedir = '\0'; // Terminate the command string before ">"
        char* file = strtok(outputRedir + (append ? 2 : 1), " \t"); // Skip ">" or ">>"
        if (file == NULL) {
            printf("\033[1;31m Oopsie Woopsie : No output file provided! \033[0m\n");
            free(cmndCopy);
            return 1;
        }

        // Open the file for redirection
        if (append) {
            fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0644);
        } else {
            fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        }

        if (fd < 0) {
            perror("Error opening output file");
            free(cmndCopy);
            return 1;
        }

        // Save current STDOUT and redirect to the file
        saved_stdout = dup(STDOUT_FILENO);
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }

    char* tokens = strtok(cmnd, " \t");
    if (tokens == NULL) {
        if (outputRedir) {
            dup2(saved_stdout, STDOUT_FILENO); // Restore original STDOUT
            close(saved_stdout);
        }
        free(cmndCopy);
        return 1;
    }

    if (!strcmp(tokens, "hop")) {
        // Hop Command
        tokens = strtok(NULL, " \t");
        isInvalidCommand = callHop(tokens);
    } else if (!strcmp(tokens, "reveal")) {
        // Reveal Command
        tokens = strtok(NULL, " \t");

        int a, l;
        a = l = 0;
        while (tokens && tokens[0] == '-') {
            if (strchr(tokens, 'a')) {
                a = 1;
            }
            if (strchr(tokens, 'l')) {
                l = 1;
            }
            tokens = strtok(NULL, " \t");
        }

        isInvalidCommand = reveal(a, l, path(tokens));
    } else if (!strcmp(tokens, "log")) {
        // Log Command
        tokens = strtok(NULL, " \t");
        callLog(tokens);
    } else if (!strcmp(tokens, "proclore")) {
        // Proclore Command
        tokens = strtok(NULL, " \t");
        isInvalidCommand = pookieLore(tokens);
    } else if (!strcmp(tokens, "seek")) {
        // Seek Command
        tokens = strtok(NULL, " \t");
        isInvalidCommand = seekHelp(tokens);
    } else if (!strcmp(tokens, "echo") || !strcmp(tokens, "wc") || !strcmp(tokens, "cat")) {
        // Seek Command
        isInvalidCommand = redirection(tokens);
    } else if (!strcmp(tokens, "activities")) {
        // Activities Command
        actities(HOME);
    } else if (!strcmp(tokens, "ping")) {
        // Signals Command
        tokens = strtok(NULL, " \t");
        int pid = atoi(tokens);
        tokens = strtok(NULL, " \t");
        int sig = atoi(tokens);
        isInvalidCommand = ping(pid, sig);
    } else if (!strcmp(tokens, "fg")) {
        // fg Command
        tokens = strtok(NULL, " \t");
        if (tokens == NULL) {
            printf("\033[1;31m Oopsie Woopsie : No process ID provided! \033[0m\n");
            if (outputRedir) {
                dup2(saved_stdout, STDOUT_FILENO); // Restore original STDOUT
                close(saved_stdout);
            }
            free(cmndCopy);
            return 1;
        }
        isInvalidCommand = fg(atoi(tokens));
    } else if (!strcmp(tokens, "bg")) {
        // bg Command
        tokens = strtok(NULL, " \t");
        if (tokens == NULL) {
            printf("\033[1;31m Oopsie Woopsie : No process ID provided! \033[0m\n");
            if (outputRedir) {
                dup2(saved_stdout, STDOUT_FILENO); // Restore original STDOUT
                close(saved_stdout);
            }
            free(cmndCopy);
            return 1;
        }
        isInvalidCommand = bg(atoi(tokens));
    } else if (!strcmp(tokens, "neonate")) {
        // Neonate Command
        tokens = strtok(NULL, " \t");
        if (tokens == NULL || strcmp(tokens, "-n")) {
            printf("\033[1;31m Oopsie Woopsie : Flag not found! \033[0m\n");
            if (outputRedir) {
                dup2(saved_stdout, STDOUT_FILENO); // Restore original STDOUT
                close(saved_stdout);
            }
            free(cmndCopy);
            return 1;
        }

        tokens = strtok(NULL, " \t");
        if (tokens == NULL) {
            printf("\033[1;31m Oopsie Woopsie : No time_arg provided! \033[0m\n");
            if (outputRedir) {
                dup2(saved_stdout, STDOUT_FILENO); // Restore original STDOUT
                close(saved_stdout);
            }
            free(cmndCopy);
            return 1;
        }

        isInvalidCommand = neonate(atoi(tokens));
    } else if (!strcmp(tokens, "iMan")) {
        // iMAn Command
        tokens = strtok(NULL, " \t");
        if (tokens == NULL) {
            printf("\033[1;31m Oopsie Woopsie : No command provided! \033[0m\n");
            if (outputRedir) {
                dup2(saved_stdout, STDOUT_FILENO); // Restore original STDOUT
                close(saved_stdout);
            }
            free(cmndCopy);
            return 1;
        }
        isInvalidCommand = getManual(tokens);
        sleep(1);
    } else if (!strcmp(tokens, "urmomgey")) {
        // urmomgey Command
        printf("\033[1;35m Ah, I see! Finally, a man of culture. \033[0m\n");
    } else if (!strcmp(tokens, "avilol")) {
        // avilol Command
        printf("\033[1;35m Yup, that's me. What about it? \033[0m\n");
    } else if (!strcmp(tokens, "exit")) {
        // Exit Command
        exit(0);
    } else {
        // System Command
        isInvalidCommand = bashCommand(cmndCopy, HOME);
    }

    // Restore STDOUT if it was redirected
    if (outputRedir) {
        dup2(saved_stdout, STDOUT_FILENO); // Restore original STDOUT
        close(saved_stdout);
    }

    free(cmndCopy);
    return isInvalidCommand;
}

// Displaying Background Process Output
void displayBGOutput() {
    char* bgOutputPath = (char*) malloc(256);
    if (bgOutputPath == NULL) {
        printf("\033[1;31m Oopsie Woopsie : Memory error detected! \033[0m\n");
        return;
    }
    strcpy(bgOutputPath, HOME);
    strcat(bgOutputPath, "/systemFiles/bgOutput.txt");
    FILE *bgOutput = fopen(bgOutputPath, "r");
    if (bgOutput == NULL) {
        printf("\033[1;31m Oopsie Woopsie : Couldn't open bgOutput file! \033[0m\n");
        return;
    }

    char* tmpOutPath = (char*) malloc(256);
    if (tmpOutPath == NULL) {
        printf("\033[1;31m Oopsie Woopsie : Memory error detected! \033[0m\n");
        return;
    }
    strcpy(tmpOutPath, HOME);
    strcat(tmpOutPath, "/systemFiles/tempOutput.txt");
    FILE *tempOutput = fopen(tmpOutPath, "w");
    if (tempOutput == NULL) {
        printf("\033[1;31m Oopsie Woopsie : Couldn't open tempOutput file! \033[0m\n");
        fclose(bgOutput);
        return;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;    
    while ((read = getline(&line, &len, bgOutput)) != -1) {
        size_t lineLen = strlen(line);
        if (lineLen > 0 && line[lineLen - 1] == '\n') {
            line[lineLen - 1] = '\0';  // Remove newline character
        }
    
        // Extract the first word (PID)
        char pid_str[32];
        sscanf(line, "%31s", pid_str);
        pid_t pid = atoi(pid_str); // Convert pid string to integer
    
        int status;
        pid_t result = waitpid(pid, &status, WNOHANG);
    
        if (result == 0) {
            // Process is still running, write the line to the temporary file
            fprintf(tempOutput, "%s\n", line);
        } else if (result == -1) {
            // Error occurred, assume process ended abnormally
            char command[256];
            sscanf(line, "%*s %*s %255s", command);
            printf("\033[1;33m %s exited abnormally (%s) \033[0m\n", command, pid_str);
            fflush(stdout);
        } else {
            // Process has changed state
            if (WIFEXITED(status)) {
                // Process exited normally
                char command[256];
                sscanf(line, "%*s %*s %255s", command);
                printf("\033[1;33m %s exited normally (%s) \033[0m\n", command, pid_str);
                fflush(stdout);
            }
            else if (WIFSIGNALED(status)) {
                // Process was terminated by a signal
                char command[256];
                sscanf(line, "%*s %*s %255s", command);
                printf("\033[1;33m %s was terminated by signal %d (%s) \033[0m\n", command, WTERMSIG(status), pid_str);
                fflush(stdout);
            }
            else if (WIFSTOPPED(status)) {
                // Process was stopped by a signal
                char command[256];
                sscanf(line, "%*s %*s %255s", command);
                printf("\033[1;33m %s was stopped by signal %d (%s) \033[0m\n", command, WSTOPSIG(status), pid_str);
                fflush(stdout);
            }
        }
    }

    fclose(bgOutput);
    fclose(tempOutput);
    if (line) {
        free(line);
    }

    // Replace the original file with the temporary file
    char* fPath = (char*) malloc(256);
    char* tPath = (char*) malloc(256);
    strcpy(fPath, HOME);
    strcat(fPath, "/systemFiles/bgOutput.txt");
    strcpy(tPath, HOME);
    strcat(tPath, "/systemFiles/tempOutput.txt");
    if (remove(fPath) != 0) {
        printf("\033[1;31m Oopsie Woopsie : Couldn't remove bgOutput file! \033[0m\n");
    }
    else if (rename(tPath, fPath) != 0) {
        printf("\033[1;31m Oopsie Woopsie : Couldn't rename tempOutput file! \033[0m\n");
    }
}

// Signal handler for SIGINT (Ctrl+C)
void sigintHandler(int) {
    pid_t fg_process_pgid = tcgetpgrp(STDIN_FILENO);

    // Check if a foreground process group exists and it's not the shell itself
    if (fg_process_pgid != -1 && fg_process_pgid != getpgrp()) {
        kill(-fg_process_pgid, SIGINT);
    }
    
    // Print a newline after the ^C
    printf("\n");
}

// Function to set up signal handlers
void setupSignalHandlers() {
    struct sigaction sa;
    sa.sa_handler = sigintHandler;  // Set handler function
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    // Set up SIGINT handler (Ctrl+C)
    sigaction(SIGINT, &sa, NULL);
}

int main() {
    // Set up signal handlers for Ctrl+C
    setupSignalHandlers();

    // Setting up the home and current directories
    while (!getcwd(HOME, sizeof(HOME))) {                                       // From the unistd.h library
        printf("\033[1;31m Oopsie Woopsie : getcwd() error \033[0m\n");
        printf("Retrying in 3 seconds...\n");
        sleep(3);
    }
    getcwd(cwd, sizeof(cwd));
    strcpy(prev, cwd);

    // Creating activities and bgOutput files
    char* aPath = (char*) malloc(256);
    char* bPath = (char*) malloc(256);
    strcpy(aPath, HOME);
    strcat(aPath, "/systemFiles/activities.txt");
    strcpy(bPath, HOME);
    strcat(bPath, "/systemFiles/bgOutput.txt");
    FILE *actFile = fopen(aPath, "w");
    FILE *bgfile = fopen(bPath, "w");
    if (actFile == NULL || bgfile == NULL) {
        printf("\033[1;31m Oopsie Woopsie : Couldn't create neccesary system files! \033[0m\n");
        return 1;
    }
    fclose(actFile);
    fclose(bgfile);

    // Setting up aliases
    aliasList aliasHead = getAliases();
    funcList funcHead = getFuncs();
    
    // Main loop for the shell
    while (1) {
        // Display user prompt
        prompt();

        // Command Input
        char cmnd[4096];
        if (fgets(cmnd, sizeof(cmnd), stdin) == NULL) {
            if (feof(stdin)) {
                // Handle Ctrl+D (EOF)
                printf("\033[1;36m\nExiting shell...\n\033[0m");
                break;
            }
            else {
                // Handle other errors or signals like Ctrl+C
                printf("\033[1;31m Oopsie Woopsie : Ctrl+C is disabled. Enter 'exit' \033[0m\n");
                clearerr(stdin);
                continue;
            }
        }
        strcpy(cmnd, trim(cmnd));  // Trim whitespace from the input

        // Check for background process output, if any
        displayBGOutput();

        // Check for aliases
        aliasList tmpAlias = aliasHead;
        while (tmpAlias) {
            if (!strcmp(tmpAlias->alias, cmnd)) {
                strcpy(cmnd, tmpAlias->command);
                printf("\033[1;33m Executing: %s \033[0m\n", cmnd);
                break;
            }
            tmpAlias = tmpAlias->next;
        }

        // Check for functions
        funcList tmpFunc = funcHead;
        while (tmpFunc) {
            int N = strlen(tmpFunc->func);
            if (!strncmp(tmpFunc->func, cmnd, N)) {
                char* tmp = strdup(tmpFunc->command);
                char* replacement = (char*) malloc(128);
                
                if (cmnd[N] == '(' && cmnd[N + 1] != ')') {
                    int i = N + 1;
                    int j = 0;
                    while (cmnd[i] != ')' && cmnd[i] != '\0' && j < 127) {
                        replacement[j] = cmnd[i];
                        i++;
                        j++;
                    }
                    replacement[j] = '\0';
                }
                else {
                    strcpy(replacement, "Sugondese");
                }
                
                memset(cmnd, 0, sizeof(cmnd));
                size_t len = strlen(tmp);
                for (size_t i = 0; i < len; i++) {
                    if (i <= len - 4 && tmp[i] == '"' && tmp[i+1] == '$' && tmp[i+2] == '1' && tmp[i+3] == '"') {
                        strcat(cmnd, replacement);
                        i += 3;
                    } else {
                        strncat(cmnd, &tmp[i], 1);
                    }
                }

                printf("\033[1;33m Executing: %s \033[0m\n", cmnd);
                break;
            }
            tmpFunc = tmpFunc->next;
        }

        // Extracting individual commands
        char* urmomgey;
        char* tokens = strtok_r(cmnd, ";", &urmomgey);
        while (tokens) {
            char* gay = strdup(tokens);

            // Remove leading and trailing whitespace
            while (isspace((unsigned char)*gay)) {
                gay++;
            }
            char* end = gay + strlen(gay) - 1;
            while (end > gay && isspace((unsigned char)*end)) {
                end--;
            }
            end[1] = '\0';

            char* cmndCopy = strdup(gay);
            char* ungay = strdup(gay);

            int isInvalidCommand = callCommand(ungay);
            if (!isInvalidCommand) {
                logToFile(cmndCopy, HOME);
            }
            
            tokens = strtok_r(NULL, ";", &urmomgey);
        }   
    }

    return 0;
}
