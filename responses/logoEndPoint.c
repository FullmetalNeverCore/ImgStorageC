#include "logoEndPoint.h"


void logoEP(int client_sock)
{
    char response[8192]; 
    snprintf(response, sizeof(response), 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n\r\n"
        "<html><body><pre>%s</pre></body></html>", 
        ascii_art);
    write(client_sock, response, strlen(response));
    close(client_sock);
}
