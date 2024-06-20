/*
   C header file related to listing inbox messages
*/

#ifndef LIST_H
#define LIST_H

void listInboxMails(int *tag, int sockfd, char *folder);

int getMailCount(int *tag, int sockfd, char *folder);

#endif 