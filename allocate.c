#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pqueue.h"
#include "cpu.h"

int main(int argc, char** argv) {
    int file_index, num_processor = 0;

    for(int i = 1; i < argc; i++) {
        if(!strcmp(argv[i], "-f")) {
            file_index = i + 1;
        }
        if(!strcmp(argv[i], "-p")) {
            num_processor = atoi(argv[i+1]);
        }
    }
    char* filename = argv[file_index];
    
    // printf("%s, %d\n",filename,num_processor);
    
    return 0; 
}