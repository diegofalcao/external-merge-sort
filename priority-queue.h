//
//  priority-queue.h
//  merge-sort-external
//
//  Created by Diego Falcão on 6/2/15.
//  Copyright (c) 2015 Diego Falcão. All rights reserved.
//

#ifndef __priority_queue_h__
#define __priority_queue_h__

typedef struct heapNode {
    int32_t value;
    int32_t KFileIndex;
} heapNode;

typedef struct PQ {
    heapNode* heap;
    int32_t size;
} PQ;

void initQueue(PQ *q, int32_t n);
void enqueue(heapNode node, PQ *q);
heapNode dequeue(PQ *q);
    
#endif