//
//  priority-queue.h
//  merge-sort-external
//
//  Created by Diego Falcão on 6/2/15.
//  Copyright (c) 2015 Diego Falcão. All rights reserved.
//

#ifndef __priority_queue_h__
#define __priority_queue_h__

typedef struct heap_node {
    int32_t value;
    int32_t KFile_index;
} heap_node;

typedef struct PQ {
    heap_node *heap;
    int32_t size;
} PQ;

void init_queue(PQ *q, int32_t n);
void enqueue(heap_node node, PQ *q);
heap_node dequeue(PQ *q);
    
#endif