/*
    FETCH EMAIL CONTENT
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>
#include <stdbool.h>

#include "retrieve.h"
#include "login.h"

/* Retrieve mail contents */
char *retrieveEmail(int *tag, int sockfd, char *messageNum) {
    char req[BUFFER_SIZE];
    char *res = (char *)malloc(sizeof(char) * BUFFER_SIZE);
    memset(res, 0, BUFFER_SIZE);    

    // IMAP command
    snprintf(req, BUFFER_SIZE, "A%02d FETCH %s BODY.PEEK[]\r\n", ++(*tag), messageNum);
    write(sockfd, req, strlen(req));

    // Allocate buffer to read the initial response
    if (!res) {
        fprintf(stderr, "Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    // Set socket to non-blocking mode
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    int totalBytesRead = 0;
    
    while (true) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(sockfd, &read_fds);
        struct timeval timeout = { 0, 100000 }; // 100ms timeout

        int ret = select(sockfd + 1, &read_fds, NULL, NULL, &timeout);
        if (ret == 0) {
            break;
        } else if (ret < 0) {
            fprintf(stderr, "select"); // print error message
            break;
        }

        int bytes = read(sockfd, res + totalBytesRead, BUFFER_SIZE - totalBytesRead - 1);
       

        if (strstr(res, "BAD") != NULL) {
            fprintf(stdout, "Message not found\n");

            exit(3);
        }

        int rawbytes = 0;
        sscanf(res, "* %*s FETCH (BODY[] {%d}", &rawbytes);

        totalBytesRead += bytes;

        // Check if the entire response has been received
        int byteCount = 0;
        if (totalBytesRead >= rawbytes) {
            char *marker = "}\r\n";
            char *parse = strstr(res, marker);
            if (parse != NULL) {
                parse += strlen(marker);
                while (*parse != '\0' && byteCount < rawbytes) {
                    if (strncmp(parse, "\r\n", 2) == 0) {
                        parse+=2;
                        byteCount+=2;
                        continue;
                    }
                    //fprintf(stderr, "%c", *parse);
                    parse++;
                    byteCount++;
                } if (byteCount >= rawbytes) {
                    *parse = '\0'; 
                    parse -= byteCount;
                   // fprintf(stderr, "parsed: %s\n", parse);
                }
            }
            char *content = strdup(parse);
            free(res);
            return content;
        }
    }
    free(res);
    return NULL;
}