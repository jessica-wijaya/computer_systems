/*
    PARSE EMAIL HEADER
*/

#define _GNU_SOURCE

#include <string.h>
#include <ctype.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>
#include <stdbool.h>

#include "parse.h"
#include "login.h"

/* Parse header */
char *parseHeader(int *tag, int sockfd, char *messageNum) {
    char req[BUFFER_SIZE];
    char *res = (char *)malloc(sizeof(char) * BUFFER_SIZE);

    if (!res) {
        fprintf(stderr, "Failed to allocate memory\n");
        exit(EXIT_FAILURE);
    }

    memset(res, 0, BUFFER_SIZE);    

    // IMAP command
    snprintf(req, BUFFER_SIZE, "A%02d FETCH %s BODY.PEEK[HEADER.FIELDS (FROM TO DATE SUBJECT)]\r\n", ++(*tag), messageNum);
    write(sockfd, req, strlen(req));
    read(sockfd, res, BUFFER_SIZE);
    unfoldHeaders(res);

    if (strstr(res, "BAD") != NULL) {
        fprintf(stdout, "Message not found\n");

        exit(3);
    }
    
    char *from = extract(res, "From");
    char *to = extract(res, "To");
    char *date = extract(res, "Date");
    char *subject = extract(res, "Subject");
    
    fprintf(stdout, "From: %s\n", from);
    if (to == NULL) {
        fprintf(stdout, "To:\n");
    } else {
        fprintf(stdout, "To: %s\n", to);
    }
    fprintf(stdout, "Date: %s\n", date);
    if (subject == NULL) {
        fprintf(stdout, "Subject: <No subject>\n");
    } else {
       fprintf(stdout, "Subject: %s\n", subject);
    }

    // Free memory
    free(from);
    free(to);
    free(date);
    free(subject);
    free(res);

    return NULL;
}

/* Parse subject  */
char *parseSubject(int *tag, int sockfd, char *messageNum) {
    char req[BUFFER_SIZE];
    char *res = (char *)malloc(sizeof(char) * BUFFER_SIZE);

    if (!res) {
        fprintf(stderr, "Failed to allocate memory\n");
        exit(EXIT_FAILURE);
    }

    memset(res, 0, BUFFER_SIZE);   

    // IMAP command
    snprintf(req, BUFFER_SIZE, "A%02d FETCH %s BODY.PEEK[HEADER.FIELDS (FROM TO DATE SUBJECT)]\r\n", ++(*tag), messageNum);
    write(sockfd, req, strlen(req));
    read(sockfd, res, BUFFER_SIZE);
    unfoldHeaders(res);

    char *subject = extract(res, "Subject");
    
    if (subject == NULL) {
        fprintf(stdout, "<No subject>\n");
    } else {
       fprintf(stdout, "%s\n", subject);
    }

    // Free memory
    free(subject);
    free(res);

    return NULL;
}

/* Unfold separated headers */
void unfoldHeaders(char *headers) {
    int read_pos = 0, write_pos = 0;
    int length = strlen(headers);

    while (read_pos < length) {
        // Check for CRLF followed by a whitespace
        if (headers[read_pos] == '\r' && headers[read_pos + 1] == '\n' &&
            (headers[read_pos + 2] == ' ' || headers[read_pos + 2] == '\t')) {
            // Skip the CRLF
            read_pos += 2;
        }

        headers[write_pos++] = headers[read_pos++];
    }

    headers[write_pos] = '\0';
}

/* Extract content of header */
char *extract(char *header, const char *headerField) {
    char searchHeader[BUFFER_SIZE]; 
    snprintf(searchHeader, BUFFER_SIZE, "%s:", headerField);
    char *start = strcasestr(header, searchHeader);

    if (start) {
        start += strlen(searchHeader);
        while (*start == ' ' || *start == '\t') {
            start++;
        } 
        char *end = strstr(start, "\r\n");
        
        if (end) {
            // Remove trailing spaces
            while (end > start && (end[-1] == ' ' || end[-1] == '\t')) {
                end--; 
            }
            char *value = malloc(end - start + 1);
            
            if (value) {
                strncpy(value, start, end - start);
                value[end - start] = '\0';
                return value;
            }
        }
    }

    return NULL;
}
