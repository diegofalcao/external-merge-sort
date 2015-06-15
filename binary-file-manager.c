//
//  binary-file-manager.c
//  merge-sort-external
//
//  Created by Diego Falcão on 5/31/15.
//  Copyright (c) 2015 Diego Falcão. All rights reserved.
//

#include<stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdint.h>

#include "binary-file-manager.h"

int32_t generate_random_number () {
    int32_t number = rand();
    
    return number;
}

int32_t write_binary_file (char *file_name, long total_size) {
    FILE *file = fopen(file_name, "wb");
    
    long file_size = 0;
    
    if (!file) {
        printf("Unable to open file!");
        
        return 1;
    }
    
    while (file_size < total_size) {
        
        int32_t number = generate_random_number();
        
        fwrite(&number, sizeof(int), 1, file);
        
        file_size = file_size + sizeof(int);
    }
    
    fclose(file);
    
    return 0;
    
}

int32_t read_binary_file(char *file_name) {
    printf("\nFILENAME: %s\n", file_name);
    
    FILE *file = fopen(file_name, "rb");
    
    if (!file) {
        printf("Unable to open file!");
        
        return 1;
    }
    
    int32_t number;
    
    while (fread(&number, sizeof(int), 1, file) == 1) {
        printf("%d\n", number);
    }
    
    fclose(file);
    
    return 0;
}

int32_t main(int32_t argc, char *argv[]) {
    
    /* argc should be 4 for correct execution */
    if (argc > 1)
    {
        if (strcmp(argv[1], "-w") == 0 || strcmp(argv[1], "-W") == 0) {
            if (argc != 4) {
                /* We print32_t argv[0] assuming it is the program name */
                printf("\nUsage: %s [-r/-w] <file_name> <size_in_mb>\n", argv[0]);
                
                return 1;
            }
            
            FILE *file = fopen(argv[2], "rb");
            
            if (file != NULL) {
                printf("\nFile already exists! Skipping the creation...\n");
                
                fclose(file);
                
                return 0;
            }
            
            write_binary_file(argv[2], atol(argv[3]) * 1000 * 1000);
        } else if (strcmp(argv[1], "-r") == 0 || strcmp(argv[1], "-R") == 0) {
            if (argc != 3) {
                /* We print32_t argv[0] assuming it is the program name */
                printf("\nUsage: %s [-r/-w] <file_name>\n", argv[0]);
                
                return 1;
            }

            read_binary_file(argv[2]);
        } else {
            /* We print32_t argv[0] assuming it is the program name */
            printf("\nUsage: %s <file_name> <size_in_mb>\n", argv[0]);
            
            return 1;
        }
    } else {
        /* We print32_t argv[0] assuming it is the program name */
        printf("\nUsage: %s <file_name> <size_in_mb>\n", argv[0]);
        
        return 1;
    }
    
    return 0;
}