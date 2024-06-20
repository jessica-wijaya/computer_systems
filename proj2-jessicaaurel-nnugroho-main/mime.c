/*
    MIME DECODING
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

#include "login.h"
#include "retrieve.h"
#include "parse.h"
#include "mime.h"

/* Extract boundary from fetched mail */
char *extractBoundary(char *contentType) {
    if (!contentType) {
        return NULL;
    }
    // search for the boundary
    char *boundaryPrefix = "boundary=";
    char *start = strcasestr(contentType, boundaryPrefix);
    if (!start) {
        return NULL;
    }
    start += strlen(boundaryPrefix);

    // check if the boundary is quoted
    if (*start == '"') {
        start++; // skip quote
    }

    char *end = start;
    // find the end of the boundary string
    while (*end && *end != '"' && *end != ';' && *end != '\r' && *end != '\n') {
        end++;
    }

    // allocate memory for the boundary string
    char *boundary = malloc(end - start + 1);
    if (boundary) {
        strncpy(boundary, start, end - start);
        boundary[end - start] = '\0';
    }

    return boundary;
}

/* Parse mime contents */
void parseMime(char *content, char *boundary) {
    char *allowedEncoding = "quoted-printable 7bit 8bit";
    char *allowedType = "text/plain; charset=UTF-8\r\n";
    char *startBoundary = malloc(strlen(boundary) + strlen("\r\n--\r\n") + 1);
    char *endBoundary = malloc(strlen(boundary) + strlen("\r\n--\r\n") + 3);
    if (!startBoundary || !endBoundary) {
        fprintf(stderr, "Failed to allocate memory\n");
        free(startBoundary);
        free(endBoundary);
        return;
    }
    snprintf(startBoundary, strlen(boundary) + strlen("\r\n--\r\n") + 1, "\r\n--%s\r\n", boundary);
    snprintf(endBoundary, strlen(boundary) + strlen("\r\n--\r\n") + 3, "\r\n--%s--\r\n", boundary);
    
    char *start = strstr(content, startBoundary);
    if (!start) {
        fprintf(stderr, "Start boundary not found\n");
        free(startBoundary);
        free(endBoundary);
        return;
    }
    start += strlen(startBoundary);

    // Check content-type and content transfer encoding
    char *temp = malloc(strlen(start) + 1);
    memcpy(temp, start, strlen(start) + 1); 
    unfoldHeaders(temp);

    char *encode = extract(temp, "Content-Transfer-Encoding");
    char *type = extract(temp, "Content-Type");
    char *headerEnd = strstr(start, "\r\n\r\n");
    if (headerEnd) {
        start = headerEnd + 4;
    }

    if (!encode || strstr(allowedEncoding, encode) == NULL || !type || strstr(allowedType, type) == NULL) {
        fprintf(stderr, "Required headers not found or do not match allowed values\n");
        free(encode);
        free(type);
        free(startBoundary);
        free(endBoundary);
        free(temp);
        return;
    }

    // Print contents
    int bytes = 0;
    while (strcmp(start, endBoundary)) {
        if (strncmp(start, startBoundary, strlen(startBoundary)) == 0) {
            if (strstr(start, type) == 0 || strstr(start, encode) == 0) {
                *start = '\0'; 
                start -= bytes;
                fprintf(stdout, "%s", start);
                break;
            }
        }
        if (strncmp(start, "\r\n", 2) == 0) {
            start += 2;
            bytes += 2;
            continue;
        }
        start++;
        bytes++;
    } 

    free(startBoundary);
    free(endBoundary);
    free(temp);
    free(encode);
    free(type);
}