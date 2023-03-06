#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

struct NameCounts {
    char name[30];
    int count;
    int nunNames;
};

int main(int argc, char *argv[]) {
    int pfds[2]; //pipe file descriptor
    char name[30];
    struct NameCounts aggregateNameCount[100];
    struct NameCounts nameCounter[100];
    int lineCounter = 0;

    //If not the program exits
    if(argc == 1){
        return 0;
    }

    // This for loop sets count in each element in the nameCounter data structure to 0.
    for (int j = 0; j < 100; j++) {
        nameCounter[j].count = 0;
    }
    nameCounter[0].nunNames = 0;

    pipe(pfds);

    for(int i=1; i<argc; i++) {
        int childid = fork();

        if(childid==0) {
            FILE *fp;
            fp = fopen(argv[i], "r");

            // This checks if the file is null. 
            if (fp == NULL) {
                printf("cannot open file\n");
                exit(1);
            }

            
            while (fgets(name, 30, fp)) {
                bool duplicate = false;
                if (name[0] == '\n' || name[1] == '\n') {
                    lineCounter++;
                    fprintf(stderr, "Warning - file %s line %d is empty.\n", argv[i], lineCounter);
                }
                else {
                    for (int j = 0; j < 30; j++) {

            
                        if (name[j] == '\n') {
                            name[j] = '\0';
                        }
                    }
                    for (int j = 0; j < nameCounter[0].nunNames; j++) {
                        if (strcmp(name, nameCounter[j].name) == 0) {
                            nameCounter[j].count++;
                            duplicate = true;
                            break;
                        }
                    }
                    if (!duplicate) {
                        nameCounter[nameCounter[0].nunNames].count++;
                        for (int j = 0; j < 30; j++) {
                            nameCounter[nameCounter[0].nunNames].name[j] = name[j];
                        }
                        nameCounter[0].nunNames++;
                    }
                    lineCounter++;
                }
            }

            write(pfds[1], nameCounter, sizeof(nameCounter));

            exit(0);
        }
    }
    return 0;
}