#include <stdio.h>
#include <stdbool.h>
#include <string.h>
/**
 * Program takes file of name and tally frequency from file name.txt
 * Author: Vivian Letran
 * Email: vivian.letran@sjsu.edu
 * Date Created 02/17/23
 * Last Modified Date 02/20/23
 * 
 */

//define values for an array that can take up to 100 unique names, can be up to 30 character long
#define max_name 100
#define max_len 30

int main(int argc, char *argv[] ) {
    //exit if there is no file to be read
    if(argv[1] == NULL) {
        return(0);
    }
    //take in argument and read file "name.txt"
    char* filename = argv[1];
    FILE* inFile = fopen(filename, "r");
    //if file does not exist, print error
    if(!inFile) {
        printf("error: cannot open file\n");
        return 1;
    }
    
    // Create 2D arrays 1st array keeps track of unique names 2nd array keep track of their tally
    int count = 0;

    char unique[max_name][max_len];
    memset(unique, 0, max_name*max_len*sizeof(char));

    int tally[max_name];
    memset(tally, 0, max_name*sizeof(int));

    // keeps track of which line in file
    int i = 0;
    char input[max_len];

    // Read file by line, blank spaces included
    while(true) {

        ++i;

        // set a temp string
        memset(input, 0, max_len*sizeof(char));
        
        // breaks at EOF
        if(fgets(input,max_len,inFile) == NULL) {
            break;
        }

        // print error msg for empty lines
        if(input[0] == '\n') {
            fprintf(stderr, "Warning - Line %d is empty.\n", i);
            continue;
        }
	//when you read a new line from stdin,replace it with null or \0
        if (input[strlen(input) - 1] == '\n') {
            input[strlen(input) - 1] = '\0';
        }

        // Check if this name is present in the names array
        bool found = false;
        for(int j = 0; j < count; ++j) {
            // if name found increment tally
            if(!strcmp(unique[j], input)) {
                tally[j]++;
                found = true;
                break;
            }
        }

        // if name not found , add name to the array
        if(!found && count < max_name) {
            strcpy(unique[count], input);
            tally[count]++;
            count++;
        }
    }
    
    // close file after file is read completely
    fclose(inFile);

    // Print output
    for(int j = 0; j < count; ++j) {
        fprintf(stdout, "%s. %d\n",unique[j],tally[j]);
    }

    return 0;
}
