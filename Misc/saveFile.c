int saveFile(char* name, int content_length, int client_sock, char* boundary, char* full_body) {
    printf("Entering saveFile function.\n");
    printf("Name: %s, Content-Length: %d, Client Socket: %d, Boundary: %s\n", name, content_length, client_sock, boundary);

    char dir_path[1024];
    char file_path[1024];
    snprintf(dir_path, sizeof(dir_path), "./json_%s", name);
    snprintf(file_path, sizeof(file_path), "%s/%s.png", dir_path, name);

    printf("Attempting to create directory: %s\n", dir_path);
    if (create_directory_if_not_exists(dir_path) == -1) {
        perror("Failed to create directory");
        printf("Errno: %d\n", errno);
        return -1;
    }
    printf("Directory created or already exists.\n");

    printf("Attempting to open file: %s\n", file_path);
    FILE *file = fopen(file_path, "wb");
    if (file == NULL) {
        perror("Failed to open file for writing");
        printf("Errno: %d\n", errno);
        return -1;
    }
    printf("File opened successfully.\n");

    char *png_signature = "\x89PNG\r\n\x1a\n";
    int png_signature_length = 8;
    char *png_start = memmem(full_body, content_length, png_signature, png_signature_length);
    
    if (png_start) {
        printf("PNG signature found at offset: %ld\n", png_start - full_body);
        size_t png_length = content_length - (png_start - full_body);
        size_t bytes_written = fwrite(png_start, 1, png_length, file);
        if (bytes_written != png_length) {
            printf("Error writing to file. Expected: %zu, Written: %zu\n", png_length, bytes_written);
        } else {
            printf("Successfully wrote %zu bytes to file.\n", bytes_written);
        }
    } else {
        printf("PNG signature not found in the data.\n");
    }

    fclose(file);

    //checking saved file
    FILE *check_file = fopen(file_path, "rb");
    if (check_file) {
        char check_buffer[8];
        size_t read_size = fread(check_buffer, 1, 8, check_file);
        if (read_size == 8 && memcmp(check_buffer, png_signature, png_signature_length) == 0) {
            printf("Check successful: File starts with the correct PNG signature.\n");
        } else {
            printf("Error: File does not start with the correct PNG signature.\n");
        }
        fclose(check_file);
    } else {
        printf("Error: Failed to open the saved file for checking.\n");
    }

    printf("Finished saving file.\n");
    return 0;
}
