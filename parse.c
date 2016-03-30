#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]){

    FILE* ope = fopen("temp.txt", "r");
    char* line = NULL;
    size_t n;
    while(getline(&line, &n, ope) != -1){
        char* ptr = strchr(line, '\n');
        if(ptr)
            *ptr ='\0';
            if(line[0] == line[1] && line[1] == line[2] &&
                    line[2] == line[3] && line[3] == 32){
                printf("%s\n", line);
                while(getline(&line, &n, ope) != -1 && strstr(line, "rbp") == NULL);
                printf("%s\n", line);
            }
    }
}
