#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
/**
 * Program takes file of name and tally frequency from file name.txt & name2.txt
 * UPDATED ASSIGNMENT 2 -compute the name of one or more input files in a parallel using multiple processes
 * Author: Vivian Letran
 * Email: vivian.letran@sjsu.edu
 * Date Created 02/28/23
 * Last Modified Date 03/06/23
 * 
 */

//structure NameCounts takes in unique name and tally, name will take in 30 characters.
//First array element NameCounts keeps tally of unique values from file
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

    //exit if there is no file to be read
    if(argc == 1){
        return 0;
    }

    // For loop sets count in each element in the nameCounter to 0.
    for (int j = 0; j < 100; j++) {
        nameCounter[j].count = 0;
    }
    nameCounter[0].nunNames = 0;
    //establish the pipeline
    pipe(pfds);
    /*
    For loop iterates and will use seperate child for each file
    Tally for each file will collated into nameCounter and will be sent to parent process through pipe.*/
    for(int i=1; i<argc; i++) {
        //fork our new child process
        int childid = fork();
        //if childid is 0, then tally the name
        if(childid==0) {
            FILE *fp;
            fp = fopen(argv[i], "r");

            // This checks if the file is null. 
            if (fp == NULL) {
                printf("range:cannot open file\n");
                exit(1);
            }
            //read file by line, blank spaces included output an error message when blank
            while (fgets(name, 30, fp)) {
                bool duplicate = false;
                if (name[0] == '\n' || name[1] == '\n') {
                    lineCounter++;
                    fprintf(stderr, "Warning - file %s line %d is empty.\n", argv[i], lineCounter);
                }
                // else check for duplicate names
                else {
                    for (int j = 0; j < 30; j++) {

                    //when you read a new line from stdin,replace it with null or \0
                        if (name[j] == '\n') {
                            name[j] = '\0';
                        }
                    }
                    // Check if this name is present in the names array and update count of array nameCounter. When there is a duplicate, update count with duplicate name to create another instance
                    //break when duplicate is found and continue
                    for (int j = 0; j < nameCounter[0].nunNames; j++) {
                        if (strcmp(name, nameCounter[j].name) == 0) {
                            nameCounter[j].count++;
                            duplicate = true;
                            break;
                        }
                    }
                    // if name is not a duplicate then update count and increment lineCounter
                    if (!duplicate) {
                        nameCounter[nameCounter[0].nunNames].count++;
                        // loop reads in the name and put them into nameCounter
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
    //parent waits for child
    while ((wait(NULL)) > 0) {
        struct NameCounts currentNameCount[100];
        // reads from child into currentNameCount that tallys value from current file
        read(pfds[0], currentNameCount, sizeof(currentNameCount));
        //check for duplicates, compares the n ame in currentNameCount and the name in aggregateNameElement
            bool duplicate = false;
            for(int j=0; j<aggregateNameCount[0].nunNames; j++) {
                //when comparing string and are equal then the count value of currentNameCount is added to count value of aggregateNameCount
                if(strcmp(currentNameCount[i].name, aggregateNameCount[j].name)==0) {
                    duplicate = true;
                    aggregateNameCount[j].count+=currentNameCount[i].count;
                }
            }
            // if there is no duplicate then aggregateNameCount is set equal to currentNameCount
            // nunNames in aggregateNameCount will then be increment 
            if(!duplicate) {
                for (int k = 0; k < 30; k++) {
                    aggregateNameCount[aggregateNameCount[0].nunNames].name[k] = currentNameCount[i].name[k];
                }
                aggregateNameCount[aggregateNameCount[0].nunNames].count = currentNameCount[i].count;
                aggregateNameCount[0].nunNames++;
            }
        }
    }

    //print output
    for(int i=0; i<aggregateNameCount[0].nunNames; i++) {
        fprintf(stdout, "%s: %d\n", aggregateNameCount[i].name, aggregateNameCount[i].count);
    }

    return 0;

}
