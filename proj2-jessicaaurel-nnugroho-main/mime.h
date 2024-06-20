/*
   C header file related to decoding MIME messages
*/

#ifndef MIME_H
#define MIME_H

char *extractBoundary(char *contentType);

void parseMime(char *content, char *boundary);

#endif 