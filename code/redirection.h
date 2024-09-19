#ifndef REDIR_H
#define REDIR_H

/*
    Function to redirect input/output of a command
    @param tokens: Tokenised command
    @return 0 on success, 1 on failure
*/
int redirection(char* tokens);

// Helper function to trim leading and trailing whitespaces
char* removeWhitespace(char* str);

// Helper function to read the contents of a file into a string
char* readFileContent(char* filename);

// Helper function to get wc of a file
char* wcOutput(char* data);

// Helper function to write to a file in specified mode
int writeToFile(int writeMode, char* output, char* outputFile);

#endif