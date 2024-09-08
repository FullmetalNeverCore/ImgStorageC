#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <errno.h>
#include <fcntl.h>


//opening image in binary 
FILE* open_image(int client_sock, const char *image_path) 
{
    FILE *img_file = fopen(image_path, "rb"); 
    if (img_file == NULL) {
        perror("Error opening image");
        close(client_sock);
        return NULL;
    }
    return img_file;
}

void imgEndpoint(int client_sock,const char *image_path,int BUFFER_SIZE)
{
    FILE *img_file = open_image(client_sock,image_path);

    fseek(img_file, 0, SEEK_END);
    long img_size = ftell(img_file);
    fseek(img_file, 0, SEEK_SET);  

    // Allocate memory 
    char *img_data = malloc(img_size);
    if (img_data == NULL) {
        perror("Memory allocation failed");
        fclose(img_file);
        close(client_sock);
        return;
    }

    // Read the image content into memory
    fread(img_data, 1, img_size, img_file);
    fclose(img_file);

    char header[BUFFER_SIZE];
    snprintf(header, sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: image/jpeg\r\n"
        "Content-Length: %ld\r\n"
        "\r\n", img_size);
    write(client_sock, header, strlen(header));

    // Send the image content
    write(client_sock, img_data, img_size);

    free(img_data);
    close(client_sock);

}