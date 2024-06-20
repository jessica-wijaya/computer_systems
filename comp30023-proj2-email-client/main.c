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
#include <ctype.h>


#include "login.h"
#include "retrieve.h"
#include "parse.h"
#include "mime.h"
#include "list.h"

int main(int argc, char *argv[]) {
    char *username = NULL;
    char *password = NULL;
    char *folder = "INBOX";
    char *messageNum = "*";
    char *command = NULL;
    char *server = NULL;
    char *req = (char*) malloc(sizeof(char) * BUFFER_SIZE);
    char *res = (char*) malloc(sizeof(char) * BUFFER_SIZE);
	int tag = 0;
	int sockfd;
	char *port = "143";  // Standard IMAP port
    char *commandList = "retrieve parse list mime";

    int i = 1;
    while (i < argc && argv[i] != NULL) {
        if (strcmp(argv[i], "-u") == 0) {
            username = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "-p") == 0) {
            password = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "-f") == 0) {
            folder = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "-n") == 0) {
            messageNum = argv[i + 1];
            i++;
        } else if (strstr(commandList, argv[i]) != NULL) {
            command = argv[i];
            server = argv[i+1];
            i += 2;
        }
        i++;
    }

    // Check if data is complete
    if (username == NULL || password == NULL || folder == NULL ||
        messageNum == NULL || command == NULL || server == NULL) {
            fprintf(stdout, "Data incomplete");
            exit(1);
        }

    // Create the socket and attempt to connect
    sockfd = create_socket(port, server);
    if (sockfd < 0) {
        fprintf(stderr, "Failed to create or connect the socket\n");
        logout(&tag, sockfd, req, res);
        exit(1);
	} 

    // Check if server disconnected immediately after connection
	memset(req, 0, BUFFER_SIZE);
	int bytes_read = read(sockfd, req, BUFFER_SIZE);
    if (bytes_read == 0) {
        fprintf(stderr, "Server disconnected immediately after connection\n");
        close(sockfd);
        exit(3);
    } 

    //fprintf(stderr, "%s", req);

	// LOGIN
    if (login(&tag, sockfd, username, password) != 0) {
        fprintf(stdout, "Login failure\n");
        logout(&tag, sockfd, req, res);
        exit(3);
    }

    fprintf(stderr, "login successful\n");

	// SELECT
    snprintf(req, BUFFER_SIZE, "A%02d SELECT \"%s\"\r\n", ++tag, folder);
    write(sockfd, req, strlen(req));
	read(sockfd, res, BUFFER_SIZE);

    if (strstr(res, "OK") == NULL) {

        fprintf(stdout, "Folder not found\n");
        logout(&tag, sockfd, req, res);
		exit(3);

	} 

    fprintf(stderr, "folder selected\n");
	
    char *content = NULL;
    // retrieve
    if (strcmp(command, "retrieve") == 0) {
        content = retrieveEmail(&tag, sockfd, messageNum);
        fprintf(stdout, "%s", content);
        free(content);
        exit(0);
    }
    
    // parse
    else if (strcmp(command, "parse") == 0) {
        parseHeader(&tag, sockfd, messageNum);
    } 
    
    // mime
    else if (strcmp(command, "mime") == 0) {
        char *mimeVer = "Mime-version: 1.0";
        char *contentType = "Content-Type: multipart/alternative";

        content = retrieveEmail(&tag, sockfd, messageNum);
       
        // check if mime version and content type matched
        
        if (strcasestr(content, mimeVer) == NULL) {
            fprintf(stderr, "incorrect mime ver\n");
            logout(&tag, sockfd, req, res);
            exit(4);
        }
        if (strcasestr(content, contentType) == NULL) {
            fprintf(stderr, "incorrect content type ver\n");
            logout(&tag, sockfd, req, res);
            exit(4);
        }

        char *boundary = extractBoundary(content);
        
        parseMime(content, boundary);

        free(boundary);
        free(content);
    } else if (strcmp(command, "list") == 0) {
        listInboxMails(&tag, sockfd, folder);

    }

    // Logout and close the socket when done
    logout(&tag, sockfd, req, res);

    return 0;
}
