/*
 * Tree
 * Displays the contents of a directory, including subdirectories, with 
 * indentation corresponding to depth in the filesystem hierarchy.
 * Author: Lawrence Kim - lawrence@example.com
 */

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int show_hidden = 0;

int is_pwd_or_parent(const char *name) {
    return (strcmp(name, ".") == 0 || strcmp(name, "..") == 0);
}

void indent(int level) {
    for (int i = 0; i < level; i++) {
        printf("    ");
    }
}

void walk_dir(const char *dir_name, int level) {
    DIR *dir = opendir(dir_name);
    if (dir == NULL) {
        perror(dir_name);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (is_pwd_or_parent(entry->d_name)) {
            continue;
        }
        if (!show_hidden && entry->d_name[0] == '.') {
            continue;
        }

        size_t needed = strlen(dir_name) + 1 + strlen(entry->d_name) + 1;
        char * path = malloc(needed);
        if (path == NULL) {
            perror("malloc");
            closedir(dir);
            return;
        }

        if (dir_name[strlen(dir_name) - 1] == '/') {
            snprintf(path, needed, "%s%s", dir_name, entry->d_name);
        } else {
            snprintf(path, needed, "%s/%s", dir_name, entry->d_name);
        }

        indent(level);
        printf("%s", entry->d_name);
       
       if (entry->d_type == DT_DIR) {
           printf(":\n");
           walk_dir(path, level + 1);
       } else {
           printf("\n");
       }

        free(path);
    }
    closedir(dir);
}

int main(int argc, char * argv[]) {
    if (argc < 2) {
        return 0;
    }

    if (argc > 2 && strcmp(argv[2], "-l") == 0) {
        show_hidden = 1;
    }

    walk_dir(argv[1], 0);
    return 0;
}
