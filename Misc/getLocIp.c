#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include "getLocIp.h"

int is_private_ip(struct sockaddr_in *addr) {
    unsigned long ip = ntohl(addr->sin_addr.s_addr);

    // Check if the IP is in one of the private ranges
    if ((ip >= 0x0A000000 && ip <= 0x0AFFFFFF) ||  // 10.0.0.0/8
        (ip >= 0xAC100000 && ip <= 0xAC1FFFFF) ||  // 172.16.0.0/12
        (ip >= 0xC0A80000 && ip <= 0xC0A8FFFF)) { // 192.168.0.0/16
        return 1;
    }
    return 0;
}

char* get_local_ip() {
    struct ifaddrs *interfaces = NULL;
    struct ifaddrs *temp_addr = NULL;
    char *ipAddress = NULL; 
    int success = 0;

    success = getifaddrs(&interfaces);
    if (success == 0) {
        temp_addr = interfaces;
        while (temp_addr != NULL) {
            if (temp_addr->ifa_addr->sa_family == AF_INET) { // Check if it's IPv4
                struct sockaddr_in *addr = (struct sockaddr_in *)temp_addr->ifa_addr;
                
                if (is_private_ip(addr)) {
                    ipAddress = (char *)malloc(INET_ADDRSTRLEN * sizeof(char));  
                    if (ipAddress != NULL) {
                        inet_ntop(AF_INET, &(addr->sin_addr), ipAddress, INET_ADDRSTRLEN); //converting ip binary to readable
                    }
                    break;  // Found a private IP, no need to check further
                }
            }
            temp_addr = temp_addr->ifa_next;
        }
    } else {
        printf("Error getting interfaces\n");
    }

    // Free memory
    freeifaddrs(interfaces);

    return ipAddress;  
}




