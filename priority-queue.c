//
//  priority.queue.c
//  merge-sort-external
//
//  Created by Diego Falcão on 5/31/15.
//  Copyright (c) 2015 Diego Falcão. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#include "priority-queue.h"

void insert(heapNode aNode, heapNode* heap, int32_t size) {
    int32_t idx;
    
    heapNode tmp;
    
    idx = size + 1;
    
    heap[idx] = aNode;
    
    while (heap[idx].value < heap[idx/2].value && idx > 1) {
        tmp = heap[idx];
        
        heap[idx] = heap[idx/2];
        
        heap[idx/2] = tmp;
        
        idx /= 2;
    }
}

void shiftdown(heapNode* heap, int32_t size, int32_t idx) {
    int32_t cidx;        //index for child
    
    heapNode tmp;
    
    for (;;) {
        cidx = idx*2;
        
        if (cidx > size) {
            break;   //it has no child
        }
        
        if (cidx < size) {
            
            if (heap[cidx].value > heap[cidx+1].value) {
                ++cidx;
            }
        }
        
        //swap if necessary
        if (heap[cidx].value < heap[idx].value) {
            tmp = heap[cidx];
            heap[cidx] = heap[idx];
            heap[idx] = tmp;
            idx = cidx;
        } else {
            break;
        }
    }
}

heapNode removeMin(heapNode* heap, int32_t size) {
    heapNode rv = heap[1];
    
    heap[1] = heap[size];
    
    --size;
    
    shiftdown(heap, size, 1);
    
    return rv;
}

void enqueue(heapNode node, PQ *q) {
    insert(node, q->heap, q->size);
    
    ++q->size;
}

heapNode dequeue(PQ *q) {
    heapNode rv = removeMin(q->heap, q->size);
    
    --q->size;
    
    return rv;
}

void initQueue(PQ *q, int32_t n) {
    q->size = 0;
    q->heap = (heapNode*)malloc(sizeof(heapNode)*(n+1));
}