#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <errno.h>
#include "Misc/imgworks.c"
#include "server.h"
#include "Misc/getfolders.c"
#include "Misc/dirmanager.c"
#include <ctype.h> 
#include <netinet/tcp.h>
#include <sys/time.h>
#include "responses/okSucc.c"
#include "responses/uploadForm.c"
#include "Misc/extractions.c"
#include "Misc/contentLen.c"
#include "Misc/saveFile.c"
#include "Misc/logo.c"
#include "responses/logoEndPoint.c"
#include "con_handler.h"
#include "con_handler.c"
#include "Misc/getLocIp.h"


#define PORT 8080
#define MAX_CONN 10



int main() {
    int server_fd, client_fd, max_fd, activity;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    fd_set read_fds;
    int client_sockets[MAX_CONN] = {0}; 

    printf("%s\n", ascii_art);

    printf("C version  0xNCore @2024\n");

    
    char* locIp = get_local_ip();

    printf("%s\n",locIp);

    free(locIp);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind to address and port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Listening on port %d..\n", PORT);

    while (1) {
        FD_ZERO(&read_fds); // Clear the socket set
        FD_SET(server_fd, &read_fds); // Add server socket to set
        max_fd = server_fd;

        for (int i = 0; i < MAX_CONN; i++) {
            if (client_sockets[i] > 0) {
                FD_SET(client_sockets[i], &read_fds);
            }
            if (client_sockets[i] > max_fd) {
                max_fd = client_sockets[i];
            }
        }

        activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            perror("Select error");
        }

        if (FD_ISSET(server_fd, &read_fds)) {
            if ((client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
                perror("Accept failed");
                exit(EXIT_FAILURE);
            }

            printf("New connection, socket fd is %d, ip is: %s, port: %d\n",
                   client_fd, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            for (int i = 0; i < MAX_CONN; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = client_fd;
                    printf("Adding client to list of sockets as %d\n", i);
                    break;
                }
            }
        }

        for (int i = 0; i < MAX_CONN; i++) {
                int sock = client_sockets[i];

                if (FD_ISSET(sock, &read_fds)) {
                    handle_client(sock);
                    client_sockets[i] = 0;
                }
        }
           
    }

    close(server_fd);
    return 0;
}
