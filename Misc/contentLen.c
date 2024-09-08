int contentLen(char* buffer)
{
    char *content_length_str = strstr(buffer, "Content-Length: ");
    int content_length = 0;
    if (content_length_str) 
    {
            sscanf(content_length_str, "Content-Length: %d", &content_length);
    }
    return content_length;
}