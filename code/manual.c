#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netdb.h>

// URL encoding for special characters
void encodeURL(char* dest, const char* src, size_t max) {
    char* end = dest + max - 1;

    while (*src && dest < end) {
        if (isalnum((unsigned char)*src) || strchr("-_.~", *src)) {
            *dest++ = *src;
        } else if (dest + 3 > end) {
            break;
        } else {
            sprintf(dest, "%%%02X", (unsigned char)*src);
            dest += 3;
        }
        src++;
    }

    *dest = '\0';
    return;
}

// Function to remove HTML tags and print plain text
void printPlainText(const char* html) {
    int in_tag = 0;

    while (*html) {
        if (*html == '<') {
            in_tag = 1;
        }
        else if (*html == '>') {
            in_tag = 0;
        }
        else if (!in_tag) {
            putchar(*html);
        }
        html++;
    }
}

// Function to create an HTTP request using sockets
int getManual(char* cmnd) {
    char encodedCmnd[128];
    encodeURL(encodedCmnd, cmnd, sizeof(encodedCmnd));

    // HTTP request
    char request[512];
    snprintf(request, sizeof(request), 
             "GET /?topic=%s&section=all HTTP/1.1\r\n"
             "Host: man.he.net\r\n"
             "Connection: close\r\n\r\n", 
             encodedCmnd);

    // Create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printf("\033[1;31m Oopsie Woopsie: An error occurred while creating the socket.\033[0m\n");
        return -1;
    }

    // Resolve hostname to IP address
    struct hostent *server = gethostbyname("man.he.net");
    if (!server) {
        fprintf(stderr, "Oopsie Woopsie: No host found!\n");
        close(sock);
        return -1;
    }

    // Set up the server address struct
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80); // HTTP port
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    // Connect to the server
    if (connect(sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        printf("\033[1;31m Oopsie Woopsie: An error occurred while connecting to the server.\033[0m\n");
        close(sock);
        return -1;
    }

    // Send the HTTP request
    if (send(sock, request, strlen(request), 0) < 0) {
        printf("\033[1;31m Oopsie Woopsie: An error occurred while sending the request.\033[0m\n");
        close(sock);
        return -1;
    }

    // Read the response
    char response[4096];
    int bytes_received;
    int html_found = 0;

    while ((bytes_received = recv(sock, response, sizeof(response) - 1, 0)) > 0) {
        response[bytes_received] = '\0';

        if (!html_found) {
            const char* htmlStart = strstr(response, "<html>");
            if (htmlStart != NULL) {
                html_found = 1;
                printPlainText(htmlStart);  // Print text without HTML tags
            }
        } else {
            printPlainText(response);  // Continue printing text without HTML tags
        }
    }

    if (bytes_received < 0) {
        printf("\033[1;31m Oopsie Woopsie: An error occurred while receiving data.\033[0m\n");
    }

    // Close the socket
    close(sock);
    return 0;
}
