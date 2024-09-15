#ifndef GETLOCIP_H
#define GETLOCIP_H

#include <netinet/in.h>

int is_private_ip(struct sockaddr_in *addr);
char* get_local_ip();

#endif // GETLOCIP_H
