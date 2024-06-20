/*
    C header file related to parsing email header
*/

#ifndef PARSE_H
#define PARSE_H

char *parseHeader(int *tag,int sockfd, char *messageNum);

char *parseSubject(int *tag, int sockfd, char *messageNum);

void unfoldHeaders(char *headers);

char *extract(char *header, const char *headerField);

#endif