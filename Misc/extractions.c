char* extract_boundary(const char* request) {
    printf("Extracting boundary from request:\n%s\n", request);

    char *boundary_str = strstr(request, "boundary=");
    if (boundary_str != NULL) {
        boundary_str += 9; // Skip "boundary="
        
        char *boundary_end = strstr(boundary_str, "\r\n");
        if (boundary_end == NULL) {
            boundary_end = strstr(boundary_str, "\n");
        }
        if (boundary_end == NULL) {
            boundary_end = strchr(boundary_str, ' ');
        }
        
        if (boundary_end != NULL) {
            size_t boundary_len = boundary_end - boundary_str;
            char *boundary = (char *)malloc(boundary_len + 1);
            if (boundary != NULL) {
                strncpy(boundary, boundary_str, boundary_len);
                boundary[boundary_len] = '\0'; // Null-terminate the boundary string
                
                char *start = boundary;
                char *end = boundary + boundary_len - 1;
                while (*start && isspace(*start)) start++;
                while (end > start && isspace(*end)) *end-- = '\0';
                
                if (start != boundary) {
                    memmove(boundary, start, strlen(start) + 1);
                }
                
                printf("Extracted boundary: '%s'\n", boundary);
                return boundary;
            } else {
                perror("Failed to allocate memory for boundary");
            }
        } else {
            printf("Boundary end not found\n");
        }
    } else {
        printf("Boundary string not found\n");
    }
    return NULL;
}


char* requestBody(int content_length,int client_sock)
{
    char *body = (char *)malloc(content_length + 1);
    if (body == NULL) {
        perror("Failed to allocate memory for request body");
        return NULL; 
    }

    int body_bytes_read = 0;
    int bytes_read;
    while (body_bytes_read < content_length) {
        bytes_read = read(client_sock, body + body_bytes_read, content_length - body_bytes_read);
        if (bytes_read <= 0) {
            perror("Error reading body from socket");
            free(body); 
            return NULL;
        }
        body_bytes_read += bytes_read;
    }

    body[content_length] = '\0'; 
    return body; 
}

char* extract_path(const char *body, const char *boundary) {
    printf("Entering extract_path function.\n");
    printf("Boundary: %s\n", boundary);

    char boundary_delimiter[1024];
    snprintf(boundary_delimiter, sizeof(boundary_delimiter), "--%s", boundary);
    printf("Boundary delimiter: %s\n", boundary_delimiter);

    char *part = strstr(body, boundary_delimiter);
    if (!part) {
        printf("Boundary not found in body. First 100 characters of body:\n%.100s\n", body);
        return NULL;
    }
    
    printf("Boundary found. Offset: %ld\n", part - body);
    part += strlen(boundary_delimiter);

    char *path_header = strstr(part, "name=\"path\"");
    if (!path_header) {
        printf("'name=\"path\"' not found in body part.\n");
        return NULL;
    }

    char *header_end = strstr(path_header, "\r\n\r\n");
    if (header_end == NULL) {
        header_end = strstr(path_header, "\n\n"); // Fallback for Unix-style line endings
    }

    if (header_end != NULL) {
        char *path_value = header_end + (header_end[0] == '\r' ? 4 : 2); // Skip the \r\n\r\n or \n\n
        char *path_end = strstr(path_value, "\r\n");
        if (path_end == NULL) {
            path_end = strchr(path_value, '\n'); // Fallback for Unix-style line endings
        }

        if (path_end != NULL) {
            size_t path_length = path_end - path_value;
            char *path = (char *)malloc(path_length + 1);
            if (path != NULL) {
                strncpy(path, path_value, path_length);
                path[path_length] = '\0'; // Null-terminate the string
                printf("Extracted path: %s\n", path);
                return path;
            } else {
                printf("Failed to allocate memory for path.\n");
            }
        } else {
            printf("Failed to find the end of the path value.\n");
        }
    } else {
        printf("Failed to find the end of headers.\n");
    }

    return NULL;
}
