/*
    INBOX LISTING
*/

#define _GNU_SOURCE
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "login.h"
#include "retrieve.h"
#include "parse.h"
#include "mime.h"
#include "list.h"


/* Lists the subject of all mail in a selected inbox */
void listInboxMails(int *tag, int sockfd, char *folder) {
    int num_messages = getMailCount(tag, sockfd, folder);

    for (int i = 1; i <= num_messages; i++) {
        char *messageNum = malloc(BUFFER_SIZE);
        if (!messageNum) {
            fprintf(stderr, "Failed to allocate memory for messageNum\n");
            continue;
        }
        snprintf(messageNum, BUFFER_SIZE, "%d", i);
        fprintf(stdout, "%s: ", messageNum);
        parseSubject(tag, sockfd, messageNum);
        free(messageNum); // Free messageNum after use
    }
}

/* Returns the number of mail in a folder */
int getMailCount(int *tag, int sockfd, char *folder) {
    char req[BUFFER_SIZE];
    char *res = (char *)malloc(sizeof(char) * BUFFER_SIZE);

    if (!res) {
        fprintf(stderr, "Failed to allocate memory for res\n");
        exit(EXIT_FAILURE);
    }

    memset(res, 0, BUFFER_SIZE);

    // IMAP Command: Number of mails in a folder
    snprintf(req, BUFFER_SIZE, "A%02d STATUS %s (MESSAGES)\r\n", ++(*tag), folder);
    write(sockfd, req, strlen(req));
    read(sockfd, res, BUFFER_SIZE);

    // Parse the number of messages in the response. Example response: * STATUS Inbox (MESSAGES 3)
    char *temp = strstr(res, "MESSAGES");
    if (temp != NULL) {
        char *num_messages = temp + strlen("MESSAGES ");
        int count = atoi(num_messages);
        free(res); 
        return count;

    } else {
        fprintf(stderr, "Error: An error occurred trying to parse STATUS\n");
        free(res);
        exit(EXIT_FAILURE);
    }
}





