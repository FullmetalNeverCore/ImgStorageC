void defaultResponse(int client_sock);
int contentLen(char* buffer);
int saveFile(char* name, int content_length, int client_sock, char* boundary, char* full_body);
char* extract_path(const char *request_body, const char *boundary);
char* requestBody(int content_length,int client_sock);
char* extract_boundary(const char *request);