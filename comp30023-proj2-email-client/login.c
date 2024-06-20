/*
    C file related to connect and login to IMAP server
*/
#define _POSIX_C_SOURCE 200112L

#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 

#include "login.h"

/* Create and connect to socket */
int create_socket(char* service, char *server) {
    int s, sockfd;
    struct addrinfo hints, *res, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // Try IPv6 first, fall back to IPv4
    hints.ai_socktype = SOCK_STREAM; // Connection-mode byte streams
    hints.ai_flags = AI_PASSIVE;     // For bind, listen, accept

    s = getaddrinfo(server, service, &hints, &res);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        return -1;
    }

    // Try each address until we successfully connect
    for (p = res; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1) {
            continue; // Try next address
        }
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == 0) {
            break; // Success
        }
        close(sockfd);
    }

    

    freeaddrinfo(res);

    if (p == NULL) {
        fprintf(stderr, "Could not connect\n");
        return -1;
    }

    return sockfd;
}

/* Login to IMAP server */
int login(int *tag, int sockfd, const char *username, const char *password) {
    char req[BUFFER_SIZE];
    char res[BUFFER_SIZE];

    // Clear the buffers
    memset(req, 0, BUFFER_SIZE);
    memset(res, 0, BUFFER_SIZE);

    // LOGIN command
    snprintf(req, BUFFER_SIZE, "A%02d LOGIN %s %s\r\n", ++(*tag), username, password);
    if (write(sockfd, req, strlen(req)) < 0) {
        fprintf(stderr, "Write failed during login");
        return -1; 
    }

    if (read(sockfd, res, BUFFER_SIZE) < 0) {
        fprintf(stderr, "Read failed during login");
        return -1;
    }

    // Check if login was successful
    if (strstr(res, "OK") != NULL) {
        return 0;  // Success
    } else {
        fprintf(stderr, "Login failure\n");
        return -1;  // Fail
    }
}

/* Logout and close socket */
void logout(int *tag, int sockfd, char *req, char *res) {
    snprintf(req, BUFFER_SIZE, "A%02d LOGOUT\r\n", ++(*tag));
    write(sockfd, req, strlen(req));
    free(res);
    free(req);
    close(sockfd);
}