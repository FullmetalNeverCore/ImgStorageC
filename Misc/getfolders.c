#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "getfolders.h"

char **list_folders(const char *path, int *count) {
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
    char **folders = NULL;
    int folder_count = 0;
    int folder_capacity = 10;

    if ((dir = opendir(path)) == NULL) {
        perror("opendir");
        return NULL;
    }

    folders = (char **)malloc(folder_capacity * sizeof(char *));
    if (folders == NULL) {
        perror("malloc");
        closedir(dir);
        return NULL;
    }

    while ((entry = readdir(dir)) != NULL) {
        char full_path[MAX_NAME_LENGTH];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        if (stat(full_path, &statbuf) == -1) {
            perror("stat");
            continue;
        }

        if (S_ISDIR(statbuf.st_mode) && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, ".") != 0 && strstr(entry->d_name, "json_") != NULL) {
            if (folder_count >= folder_capacity) {
                folder_capacity *= 2;
                folders = (char **)realloc(folders, folder_capacity * sizeof(char *));
                if (folders == NULL) {
                    perror("realloc");
                    closedir(dir);
                    return NULL;
                }
            }

            folders[folder_count] = (char *)malloc((strlen(entry->d_name) + 1) * sizeof(char));
            if (folders[folder_count] == NULL) {
                perror("malloc");
                closedir(dir);
                return NULL;
            }

            //checiking if folder name contains json_ and removing it 
            char *prefix = "json_";
            char *pos = strstr(entry->d_name, prefix);

            if (pos == entry->d_name) 
            { 
                strcpy(folders[folder_count], pos + strlen(prefix)); 
            }
            else
            {
                strcpy(folders[folder_count], entry->d_name);
            }

            folder_count++;
        }
    }

    closedir(dir);

    *count = folder_count;
    return folders;
}

void free_folders(char **folders, int count) {
    printf("freeing some space..\n");
    for (int i = 0; i < count; i++) {
        free(folders[i]);
    }
    free(folders);
}
