#include <unistd.h>
#include <string.h>
#include "okSucc.h"

void okSucc(int client_sock)
{
    char *response = "HTTP/1.1 200 OK\r\n"
                         "Content-Type: text/html\r\n\r\n"
                         "<html><body><h1>Upload successful!</h1></body></html>";
    write(client_sock, response, strlen(response));
    close(client_sock);
}
