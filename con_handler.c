#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <errno.h>
#include <ctype.h> 
#include <netinet/tcp.h>
#include <sys/time.h>

#include "responses/okSucc.h"
#include "responses/uploadForm.h"
#include "Misc/logo.h"
#include "responses/logoEndPoint.h"
#include "server.h"
#include "con_handler.h"

#include "Misc/imgworks.h"

#define BUFFER_SIZE 65500  // 64 KB
#define CHUNK_SIZE 4096 

void handle_client(int client_sock) {
    char buffer[BUFFER_SIZE];
    int bytes_read, total_bytes_read = 0;
    char request[BUFFER_SIZE * 10] = {0}; // Larger buffer to accumulate request

    while ((bytes_read = read(client_sock, buffer, sizeof(buffer) - 1)) > 0) {
        strncat(request, buffer, bytes_read);
        total_bytes_read += bytes_read;

        if (strstr(request, "\r\n\r\n")) {
            break;
        }
    }

    if (bytes_read < 0) {
        perror("Error reading from socket");
        close(client_sock);
        return;
    }

    printf("Received request:\n%s\n", request);

    if (strstr(request, "GET /upload") != NULL) {
        uploadResponse(client_sock);
    }
    else if (strstr(request, "GET /status") != NULL)
    {
        logoEP(client_sock); 
    }
    else if (strstr(request, "POST /upload/creation") != NULL) {
        printf("Attempting to create folder and upload image into that folder...\n");

        int content_length = contentLen(request);
        printf("Content-Length extracted: %d\n", content_length);
        
        if (content_length > 0) {
            printf("Content-Length: %d\n", content_length);

            char *boundary = extract_boundary(request);
            if (boundary == NULL) {
                printf("Failed to extract boundary from headers.\n");
                close(client_sock);
                return;
            }

            printf("Extracted boundary: '%s'\n", boundary);

            char *full_body = requestBody(content_length, client_sock);
            if (full_body == NULL) {
                printf("Failed to read full request body.\n");
                free(boundary);
                close(client_sock);
                return;
            }

            char *path = extract_path(full_body, boundary);
            if (path == NULL) {
                printf("Failed to extract path from request body.\n");
                free(boundary);
                free(full_body);
                close(client_sock);
                return;
            }

            printf("Successfully extracted path: %s\n", path);

            printf("Calling saveFile function...\n");
            printf("Path: %s, Content-Length: %d, Client socket: %d, Boundary: %s\n", 
                   path, content_length, client_sock, boundary);
            int save_result = saveFile(path, content_length, client_sock, boundary, full_body);
            printf("saveFile function returned: %d\n", save_result);

            free(boundary);
            free(path);
            free(full_body);
            okSucc(client_sock);
        } else {
            printf("No Content-Length header found or invalid value.\n");
        }
    }
    else {
        char *method = strtok(request, " "); 
        char *endpoint = strtok(NULL, " "); 
        
        if (endpoint != NULL && strlen(endpoint) > 1) {
            char file_path[BUFFER_SIZE];
            char *name = strtok(endpoint, "/");
            if (name != NULL) {
                printf("Accessing endpoint: %s\n", name);
                snprintf(file_path, sizeof(file_path), "./json_%s/%s.png", name,name);
                printf("File path: %s\n", file_path);
                imgEndpoint(client_sock, file_path, BUFFER_SIZE);
            } else {
                const char *error_response = "HTTP/1.1 400 Bad Request\r\n\r\nNo such file";
                write(client_sock, error_response, strlen(error_response));
            }
        } else {
            const char *error_response = "HTTP/1.1 400 Bad Request\r\n\r\nIncorrect endpoint";
            write(client_sock, error_response, strlen(error_response));
        }
        close(client_sock);
    }
}