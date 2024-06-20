/*
    C header file related to IMAP server connection
*/

#ifndef LOGIN_H
#define LOGIN_H

#include <unistd.h>

#define BUFFER_SIZE 30000

int create_socket(char* service, char *server);

int login(int *tag, int sockfd, const char *username, const char *password);

void logout(int *tag, int sockfd, char *req, char *res);

#endif

