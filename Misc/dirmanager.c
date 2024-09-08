#include <sys/stat.h> 
#include <errno.h>    
#include <string.h>  

int create_directory_if_not_exists(const char *dir_path) {
    if (access(dir_path, F_OK) != -1) {
        return 0; 
    }

    if (mkdir(dir_path, 0777) == -1) {
        fprintf(stderr, "Err in dir creation: %s\n", strerror(errno));
        return -1; 
    }
    return 0; 
}