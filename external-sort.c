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
int32_t cmpFunction(const void * a, const void * b) {
    return (*(int*)a - *(int*)b);
}

/* Generates the initial k-ways */
KFile* generateKFiles(const int32_t K) {
    KFile *files = malloc((2*K) * sizeof(KFile));
    
    int32_t i;
    
    for (i = 0; i < (2 * K); i++) {
        char *charTmp = malloc(sizeof(int));
        sprintf(charTmp, "%d", i);
        
        //+1 for the zero-terminator
        char *fileName = malloc(strlen("kfile_")
                                + strlen(charTmp)
                                + strlen(".bin") + 1);
        
        strcpy(fileName, "kfile_");
        strcat(fileName, charTmp);
        strcat(fileName, ".bin");

        files[i].id = i;
        files[i].name = fileName;
        files[i].file = fopen(fileName, "w+b");
        files[i].actualSize = 0;
        files[i].actualRun.size = 0;
    }
    
    return files;
}

void printFileStructure(KFile *files, int32_t size) {
    printf("\n***********************************************");
    
    for (int32_t i = 0; i < size; i++) {
        printf("\nID: %d\n", files[i].id);
        printf("NAME: %s\n", files[i].name);
        printf("ACTUAL SIZE: %lu", files[i].actualSize);
        
        printf("\nACTUAL RUN SIZE: %zu", files[i].actualRun.size);
    }
    
    printf("\n***********************************************");
}

unsigned long getFileLength(FILE* file) {
    unsigned long fileLength;
    
    fseek(file, 0, SEEK_END);
    
    fileLength = ftell(file);
    
    fseek(file, 0, SEEK_SET);
    
    return fileLength;
}

int32_t getNumberOfRuns(unsigned long fileLength, const int32_t AVAILABLE_MEMORY) {
    int32_t numberOfRuns = ceil((float) fileLength / AVAILABLE_MEMORY);
    
    return numberOfRuns;
}

/* Generate the initial runs */
KFile* generateRuns(FILE *file,
                    const int32_t AVAILABLE_MEMORY,
                    const int32_t K) {
    KFile* files = generateKFiles(K);
    
    int32_t * buffer = NULL;
    
    int32_t numberOfTotalRuns = getNumberOfRuns(getFileLength(file), AVAILABLE_MEMORY);
    
    int32_t x;
    
    for (x = 0; x < numberOfTotalRuns; x++) {
        buffer = (int32_t *) malloc(AVAILABLE_MEMORY);
        
        size_t bytesRead = fread(buffer, 1, AVAILABLE_MEMORY, file);
        
        qsort(buffer, bytesRead / ITEM_SIZE, sizeof(int), cmpFunction);
        
        int32_t actualPosition = x % K;
        
        files[actualPosition].actualSize = files[actualPosition].actualSize
        + bytesRead;
        
        Run run;
        run.size = bytesRead;
        
        int32_t numberOfRuns;
        
        if((actualPosition + 1) == K - 1) {
            numberOfRuns = ceil((double) numberOfTotalRuns / K);
        }
        else {
            numberOfRuns = round((double) numberOfTotalRuns / K);
        }
        
        if(files[actualPosition].actualRun.size == 0) {
            files[actualPosition].actualRun = run;
        }
        
        fwrite(buffer, bytesRead, 1, files[actualPosition].file);
    }
    
    /* Seek to the beginning of the file */
    for(x = 0; x < K + 1; x++) {
        fseek(files[x].file, SEEK_SET, 0);
    }
    
    free(buffer);
    
    return files;
}

int32_t intLog(double base, double x) {
    return ceil((double) (log(x) / log(base)));
}

/* Interpolate the values and return the output file index */
int32_t interpolate(KFile *files,
                unsigned long inputFileLength,
                const unsigned long AVAILABLE_MEMORY,
                const int32_t K) {
    
    PQ priorityQueue;

    int32_t numOfinterpolationSteps = intLog(K, inputFileLength / AVAILABLE_MEMORY);
    
    int32_t countNumOfInterpolations;
    int32_t countNumOfCombinationPerInterpolation;
 
    /* It holds the actual output position index */
    int32_t actualOutputPosition = -1;
    
    for (countNumOfInterpolations = 0;
         countNumOfInterpolations < numOfinterpolationSteps;
         countNumOfInterpolations++) {
        
        int32_t numOfCombinationsPerInterpolation = numOfinterpolationSteps -
                                                countNumOfInterpolations;
        
        int32_t start, end;
        
        
        if (countNumOfInterpolations % 2 == 0) {
            start = 0;
            end = K;

            actualOutputPosition = K;

        } else {
            start = K;
            end = 2 * K;
            
            actualOutputPosition = 0;
        }
        
        for(countNumOfCombinationPerInterpolation = 0;
            countNumOfCombinationPerInterpolation < numOfCombinationsPerInterpolation;
            countNumOfCombinationPerInterpolation++) {

            int32_t count;
            
            size_t totalBytesToBeWritten = 0;
            
            initQueue(&priorityQueue, K);
 
            /* Populates the heap with the first values of the current run */
            for (count = start; count < end; count++) {

                if (files[count].actualSize == 0) {
                    continue;
                }
                
                /* skip only in the first time */
                if(countNumOfCombinationPerInterpolation == 0 &&
                   countNumOfInterpolations > 0) {
                    fseek(files[count].file, SEEK_SET, 0);
                }
                
                /* Creating the initial current run for each k-way */
                if(files[count].actualSize != 0 && files[count].actualRun.size == 0) {
                    Run run;
                    
                    if(files[count].actualSize <= AVAILABLE_MEMORY) {
                        run.size = files[count].actualSize;
                    }
                    else {
                        run.size = (countNumOfInterpolations + 1) * AVAILABLE_MEMORY;
                    }
                    
                    files[count].actualRun = run;
                }
                
                heapNode tmpHeapNode;
                tmpHeapNode.KFileIndex = count;
                
                fread(&tmpHeapNode.value, 1, ITEM_SIZE, files[count].file);
                
                totalBytesToBeWritten += files[count].actualRun.size;
                
                files[count].actualRun.size -= sizeof(int);
                files[count].actualSize -= sizeof(int);
                
                enqueue(tmpHeapNode, &priorityQueue);
            }

            size_t totalBytesWritten = 0;
            
            /* If the destination file has values and we are in the first
             * combination, clean it! 
             */
            if(countNumOfCombinationPerInterpolation == 0 &&
                countNumOfInterpolations > 0) {
                
                ftruncate(fileno(files[actualOutputPosition].file), 0);

                fseek(files[actualOutputPosition].file, SEEK_SET, 0);
            }

            while (priorityQueue.size > 0) {
                heapNode heapNode = dequeue(&priorityQueue);
                
                fwrite(&heapNode.value, sizeof(int), 1, files[actualOutputPosition].file);
                
                files[actualOutputPosition].actualSize += sizeof(int);
                
                totalBytesWritten += sizeof(int);
                
                if (files[heapNode.KFileIndex].actualRun.size == 0) {
                    continue;
                }
                
                fread(&heapNode.value, 1, ITEM_SIZE, files[heapNode.KFileIndex].file);
                
                files[heapNode.KFileIndex].actualRun.size -= sizeof(int);
                
                files[heapNode.KFileIndex].actualSize = files[heapNode.KFileIndex].actualSize - sizeof(int);
                
                enqueue(heapNode, &priorityQueue);
            }
            
            /* Generating the runs for the new output file */
            Run run;
            run.size = totalBytesWritten;
            
            if (files[actualOutputPosition].actualRun.size == 0) {
                files[actualOutputPosition].actualRun = run;
            }

            actualOutputPosition++;
        }
    }
    
    return (actualOutputPosition - 1);
    
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
    
    KFile *files = generateRuns(file, AVAILABLE_MEMORY, K);

    unsigned long inputFileLength = getFileLength(file);
    
    int32_t outputFileIndex = interpolate(files, inputFileLength, AVAILABLE_MEMORY, K);

    end = clock();
    
    double searchTimeSpent = (double) (end - begin) / CLOCKS_PER_SEC;

    /* closing the files and renaming the output file */
    for (int32_t x = 0; x < 2 * K; x++) {
        fclose(files[x].file);
        
        if (x == outputFileIndex) {
            continue;
        }
        
        remove(files[x].name);
    }

    rename(files[outputFileIndex].name, argv[2]);

    printf("\nINPUT FILE: %s\n", argv[1]);
    printf("\nAVAILABLE MEMORY: %d", AVAILABLE_MEMORY);
    printf("\nK: %d", K);
    printf("\n%lf\n", searchTimeSpent);
    
    fclose(file);
    
    return 0;
}