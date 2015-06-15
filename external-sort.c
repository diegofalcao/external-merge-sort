//
//  external-merge-sort.c
//  merge-sort-external
//
//  Created by Diego Falcão on 5/31/15.
//  Copyright (c) 2015 Diego Falcão. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "priority-queue.h"
#include "external-sort.h"

/* this function compares two numbers to verify which one is higher */
int32_t compare_function (const void * a, const void * b) {
    return (*(int*)a - *(int*)b);
}

/* Generates the initial k-ways */
KFile* generate_KFiles (const int32_t K) {
    KFile *files = malloc ((2*K) * sizeof(KFile));
    
    int32_t i;
    
    for (i = 0; i < (2 * K); i++) {
        char *tmp_char = malloc (sizeof(int));
        sprintf(tmp_char, "%d", i);
        
        //+1 for the zero-terminator
        char *file_name = malloc(strlen("kfile_")
                                + strlen(tmp_char)
                                + strlen(".bin") + 1);
        
        strcpy(file_name, "kfile_");
        strcat(file_name, tmp_char);
        strcat(file_name, ".bin");

        files[i].id = i;
        files[i].name = file_name;
        files[i].file = fopen(file_name, "w+b");
        files[i].actual_size = 0;
        files[i].actual_run.size = 0;
    }
    
    return files;
}

void print_file_structure (KFile *files, int32_t size) {
    printf("\n***********************************************");
    
    for (int32_t i = 0; i < size; i++) {
        printf("\nID: %d\n", files[i].id);
        printf("NAME: %s\n", files[i].name);
        printf("ACTUAL SIZE: %lu", files[i].actual_size);
        
        printf("\nACTUAL RUN SIZE: %zu", files[i].actual_run.size);
    }
    
    printf("\n***********************************************");
}

unsigned long get_file_length (FILE* file) {
    unsigned long fileLength;
    
    fseek(file, 0, SEEK_END);
    
    fileLength = ftell(file);
    
    fseek(file, 0, SEEK_SET);
    
    return fileLength;
}

int32_t get_number_of_runs (unsigned long file_length, const int32_t AVAILABLE_MEMORY) {
    int32_t number_of_runs = ceil((float) file_length / AVAILABLE_MEMORY);
    
    return number_of_runs;
}

/* Generate the initial runs */
KFile* generate_runs (FILE *file,
                    const int32_t AVAILABLE_MEMORY,
                    const int32_t K) {
    KFile* files = generate_KFiles(K);
    
    int32_t * buffer = NULL;
    
    int32_t number_of_total_runs = get_number_of_runs(get_file_length(file), AVAILABLE_MEMORY);
    
    int32_t x;
    
    for (x = 0; x < number_of_total_runs; x++) {
        buffer = (int32_t *) malloc(AVAILABLE_MEMORY);
        
        size_t bytes_read = fread(buffer, 1, AVAILABLE_MEMORY, file);
        
        qsort(buffer, bytes_read / ITEM_SIZE, sizeof(int), compare_function);
        
        int32_t actual_position = x % K;
        
        files[actual_position].actual_size = files[actual_position].actual_size
        + bytes_read;
        
        Run run;
        run.size = bytes_read;
        
        int32_t number_of_runs;
        
        if ((actual_position + 1) == K - 1) {
            number_of_runs = ceil((double) number_of_total_runs / K);
        }
        else {
            number_of_runs = round((double) number_of_total_runs / K);
        }
        
        if (files[actual_position].actual_run.size == 0) {
            files[actual_position].actual_run = run;
        }
        
        fwrite(buffer, bytes_read, 1, files[actual_position].file);
    }
    
    /* Seek to the beginning of the file */
    for (x = 0; x < K + 1; x++) {
        fseek(files[x].file, SEEK_SET, 0);
    }
    
    free(buffer);
    
    return files;
}

int32_t int_log (double base, double x) {
    return ceil((double) (log(x) / log(base)));
}

/* Interpolate the values and return the output file index */
int32_t interpolate(KFile *files,
                unsigned long input_file_length,
                const unsigned long AVAILABLE_MEMORY,
                const int32_t K) {
    
    PQ priority_queue;

    int32_t num_of_interpolation_steps = int_log(K, input_file_length / AVAILABLE_MEMORY);
    
    int32_t count_num_of_interpolations;
    int32_t count_num_of_combination_per_interpolation;
 
    /* It holds the actual output position index */
    int32_t actual_output_position = -1;
    
    for (count_num_of_interpolations = 0;
         count_num_of_interpolations < num_of_interpolation_steps;
         count_num_of_interpolations++) {
        
        int32_t num_of_combinations_per_interpolation = num_of_interpolation_steps -
                                                        count_num_of_interpolations;
        
        int32_t start, end;
        
        
        if (count_num_of_interpolations % 2 == 0) {
            start = 0;
            end = K;

            actual_output_position = K;

        } else {
            start = K;
            end = 2 * K;
            
            actual_output_position = 0;
        }
        
        for (count_num_of_combination_per_interpolation = 0;
            count_num_of_combination_per_interpolation < num_of_combinations_per_interpolation;
            count_num_of_combination_per_interpolation++) {

            int32_t count;
            
            size_t total_bytes_written = 0;
            
            init_queue(&priority_queue, K);
 
            /* Populates the heap with the first values of the current run */
            for (count = start; count < end; count++) {

                if (files[count].actual_size == 0) {
                    continue;
                }
                
                /* skip only in the first time */
                if (count_num_of_combination_per_interpolation == 0 &&
                   count_num_of_interpolations > 0) {
                    fseek(files[count].file, SEEK_SET, 0);
                }
                
                /* Creating the initial current run for each k-way */
                if (files[count].actual_size != 0 && files[count].actual_run.size == 0) {
                    Run run;
                    
                    if(files[count].actual_size <= AVAILABLE_MEMORY) {
                        run.size = files[count].actual_size;
                    }
                    else {
                        run.size = (count_num_of_interpolations + 1) * AVAILABLE_MEMORY;
                    }
                    
                    files[count].actual_run = run;
                }
                
                heap_node tmp_heap_node;
                tmp_heap_node.KFile_index = count;
                
                fread(&tmp_heap_node.value, 1, ITEM_SIZE, files[count].file);
                
                total_bytes_written += files[count].actual_run.size;
                
                files[count].actual_run.size -= sizeof(int);
                files[count].actual_size -= sizeof(int);
                
                enqueue(tmp_heap_node, &priority_queue);
            }

            total_bytes_written = 0;
            
            /* If the destination file has values and we are in the first
             * combination, clean it! 
             */
            if (count_num_of_combination_per_interpolation == 0 &&
                count_num_of_interpolations > 0) {
                
                ftruncate(fileno(files[actual_output_position].file), 0);

                fseek(files[actual_output_position].file, SEEK_SET, 0);
            }

            while (priority_queue.size > 0) {
                heap_node heap_node = dequeue(&priority_queue);
                
                fwrite(&heap_node.value, sizeof(int), 1, files[actual_output_position].file);
                
                files[actual_output_position].actual_size += sizeof(int);
                
                total_bytes_written += sizeof(int);
                
                if (files[heap_node.KFile_index].actual_run.size == 0) {
                    continue;
                }
                
                fread(&heap_node.value, 1, ITEM_SIZE, files[heap_node.KFile_index].file);
                
                files[heap_node.KFile_index].actual_run.size -= sizeof(int);
                
                files[heap_node.KFile_index].actual_size = files[heap_node.KFile_index].actual_size - sizeof(int);
                
                enqueue(heap_node, &priority_queue);
            }
            
            /* Generating the runs for the new output file */
            Run run;
            run.size = total_bytes_written;
            
            if (files[actual_output_position].actual_run.size == 0) {
                files[actual_output_position].actual_run = run;
            }

            actual_output_position++;
        }
    }
    
    return (actual_output_position - 1);
    
}

int32_t main(int32_t argc, char *argv[]) {
    /*
     * Number of ways (number of files)
     * This should be greater than 3 because one file is the output file and the
     * combination step considers at least 2 files.
     */

    /* argc should be 3 for correct execution */
    if (argc != 5)
    {
        /* We print32_t argv[0] assuming it is the program name */
        printf("Usage: %s <in_file_name> <out_file_name> <available_mem_in_mb> <num_of_ways>", argv[0]);
        
        return 1;
    }
    
    const int32_t K = atoi(argv[4]);
    
    int32_t AVAILABLE_MEMORY = atoi(argv[3]) * 1000 * 1000; //value in bytes
   
    FILE *file = fopen(argv[1], "rb");
    
    if (!file) {
        printf("\nUnable to open file!");
        
        return 1;
    }
    
    clock_t begin, end;
    
    begin = clock();
    
    KFile *files = generate_runs(file, AVAILABLE_MEMORY, K);

    unsigned long input_file_length = get_file_length(file);
    
    int32_t output_file_index = interpolate(files, input_file_length, AVAILABLE_MEMORY, K);

    end = clock();
    
    double time_spent = (double) (end - begin) / CLOCKS_PER_SEC;

    /* closing the files and renaming the output file */
    for (int32_t x = 0; x < 2 * K; x++) {
        fclose(files[x].file);
        
        if (x == output_file_index) {
            continue;
        }
        
        remove(files[x].name);
    }

    rename(files[output_file_index].name, argv[2]);

    printf("\nINPUT FILE: %s\n", argv[1]);
    printf("\nAVAILABLE MEMORY: %d", AVAILABLE_MEMORY);
    printf("\nK: %d", K);
    printf("\n%lf\n", time_spent);
    
    fclose(file);
    
    return 0;
}