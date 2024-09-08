void uploadResponse(int client_sock)
{
    const char *response = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n\r\n"
    "<!DOCTYPE html>"
    "<html lang=\"en\">"
    "<head><meta charset=\"UTF-8\">"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1, shrink-to-fit=no\">"
    "<link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css\">"
    "<title>Image Upload</title>"
    "<style>body {height: 100vh;display: flex;align-items: center;justify-content: center;}</style>"
    "</head>"
    "<body><div class=\"container text-center\"><div class=\"row\"><div class=\"col-md-6 mx-auto\"><h1>Image Upload</h1>"
    "<form method=\"POST\" action=\"/upload/creation\" enctype=\"multipart/form-data\"><div class=\"form-group\"><input type=\"text\" class=\"form-control\" id=\"path\" name=\"path\" placeholder=\"Enter path\" required></div>"
    "<div class=\"form-group\"><input type=\"file\" class=\"form-control-file\" name=\"image\" accept=\"image/*\" required></div>"
    "<button type=\"submit\" class=\"btn btn-primary\">Upload</button></form></div></div></div></body></html>";
    
    write(client_sock, response, strlen(response));
    close(client_sock);
}