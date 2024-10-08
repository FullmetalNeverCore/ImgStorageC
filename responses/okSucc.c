#include <unistd.h>
#include <string.h>
#include "okSucc.h"
#include "../Misc/logo.c"


void okSucc(int client_sock)
{
    char response[8192]; 

    snprintf(response, sizeof(response), 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n\r\n"
        "<html><body><h1>Upload successful!</h1><pre>%s</pre></body></html>", 
        ascii_art);
    write(client_sock, response, strlen(response));
    close(client_sock);
}
