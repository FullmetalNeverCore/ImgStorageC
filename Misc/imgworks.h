#ifndef IMGWORKS_H
#define IMGWORKS_H

#include <stdio.h>

FILE* open_image(int client_sock, const char *image_path);
void imgEndpoint(int client_sock, const char *image_path, int BUFFER_SIZE);

#endif // IMGWORKS_H