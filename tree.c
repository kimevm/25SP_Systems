#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

int show_hidden = 0;

//opens the direcotry whose name is passed
//and then in for loop reads each directory entry (as a struct dirent object) belonging to the DIR * object just oepned
//if hidden files are not to be dispalyed, skip them!
//don't use strcmp to check the first character of the name


void walk_dir(const char *dir_name, int level){
    
}


//takes file name and returns whetehr the name is that of the self-listing or parental listing
int is_pwd_or_parent(const char *name){
    return (strcmp(name, ".") == 0 || strcmp(name, "..") == 0);
    //idk if we're allowed to use strcmp here.

}
void indent(int level){
    for(int i = 0; i < level; i++){
        for(int i = 0; i < level; i++){
            putchar(' ');
        }
    }
}

int main(int argc, char *argv[]){
    if (argc < 2){
        return 0;
    }

    if(argc > 2 && strcmp(argv[2], "-l") == 0){
        show_hidden = 1;
    }

    walk_dir(argv[1], 0);
    return 0;
}