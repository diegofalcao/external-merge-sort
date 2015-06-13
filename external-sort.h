//
//  externa-merge-sort.h
//  merge-sort-external
//
//  Created by Diego Falcão on 5/31/15.
//  Copyright (c) 2015 Diego Falcão. All rights reserved.
//

#ifndef __external_sort_h__
#define __external_sort_h__

/* size of each item in bytes */
#define ITEM_SIZE 4
#define TRUE 1
#define FALSE 0

typedef struct Run {
    size_t size; //in bytes
} Run;

/* This struct represents the file structure */
typedef struct KFile {
    int32_t id;
    char *name;
    FILE *file;
    Run actualRun;
    unsigned long actualSize;
} KFile;

#endif