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
#include "Misc/getLocIp.c"

#include "Misc/imgworks.h"

#define BUFFER_SIZE 65500  // 64 KB
#define CHUNK_SIZE 4096 


void handle_client(int client_sock) {
    char buffer[BUFFER_SIZE];
    int bytes_read, total_bytes_read = 0;
    char request[BUFFER_SIZE * 10] = {0}; 

    while ((bytes_read = read(client_sock, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';
        strcat(request, buffer);
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
            char *boundary = extract_boundary(request);
            if (boundary == NULL) {
                printf("Failed to extract boundary from headers.\n");
                close(client_sock);
                return;
            }

            printf("Extracted boundary: '%s'\n", boundary);

            
            char *header_end = strstr(request, "\r\n\r\n");
            if (header_end != NULL) {
                header_end += 4;
                int header_length = header_end - request;
                int body_bytes_in_buffer = total_bytes_read - header_length;

                char *full_body = malloc(content_length + 1);
                if (full_body == NULL) {
                    perror("Memory allocation failed");
                    free(boundary);
                    close(client_sock);
                    return;
                }

                memcpy(full_body, header_end, body_bytes_in_buffer);

                int remaining_bytes = content_length - body_bytes_in_buffer;
                int offset = body_bytes_in_buffer;

                while (remaining_bytes > 0) {
                    bytes_read = read(client_sock, full_body + offset, remaining_bytes);
                    if (bytes_read <= 0) {
                        perror("Error reading request body");
                        free(full_body);
                        free(boundary);
                        close(client_sock);
                        return;
                    }
                    offset += bytes_read;
                    remaining_bytes -= bytes_read;
                }

                full_body[content_length] = '\0'; 

                char *path = extract_path(full_body, boundary);
                if (path == NULL) {
                    printf("Failed to extract path from request body.\n");
                    free(boundary);
                    free(full_body);
                    close(client_sock);
                    return;
                }

                printf("Successfully extracted path: %s\n", path);

                int save_result = saveFile(path, content_length, client_sock, boundary, full_body);
                create_json(path);
                printf("saveFile function returned: %d\n", save_result);

                free(boundary);
                free(path);
                free(full_body);
                okSucc(client_sock);
                close(client_sock);
            } else {
                printf("Failed to find end of headers.\n");
                free(boundary);
                close(client_sock);
                return;
            }
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

void create_json(char* name) { 
    char json_content[256];  
    char file_path[1024];     

    char* locIp = get_local_ip();

    if (locIp != NULL) {
        printf("Local IP: %s\n", locIp);

        snprintf(file_path, sizeof(file_path), "json_%s/img.json", name);

        snprintf(json_content, sizeof(json_content), "{\"link\":\"http://%s:5005/%s\"}", locIp, name);

        FILE *file = fopen(file_path, "w");
        if (file == NULL) {
            printf("Error: Could not create the file %s\n", file_path);
        } else {
            fprintf(file, "%s\n", json_content);

            fclose(file);

            printf("File %s created with JSON content.\n", file_path);
        }

        free(locIp);
    } else {
        printf("Error: Could not retrieve local IP address.\n");
    }
}

