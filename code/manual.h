#ifndef MANUAL_H
#define MANUAL_H

/*
    Function to get the manual of the program
    @param tokens: Command name
    @return 0 on success, 1 on failure
*/
int getManual(char* cmnd);

// URL encoding for special characters
void encodeURL(char *dest, const char *src, size_t max);

// Function to remove HTML tags and print plain text
void printPlainText(const char* html);

#endif