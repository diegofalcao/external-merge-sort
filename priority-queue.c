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

void insert (heap_node a_node, heap_node *heap, int32_t size) {
    int32_t idx;
    
    heap_node tmp;
    
    idx = size + 1;
    
    heap[idx] = a_node;
    
    while (heap[idx].value < heap[idx/2].value && idx > 1) {
        tmp = heap[idx];
        
        heap[idx] = heap[idx/2];
        
        heap[idx/2] = tmp;
        
        idx /= 2;
    }
}

void shift_down (heap_node *heap, int32_t size, int32_t idx) {
    int32_t cidx;        //index for child
    
    heap_node tmp;
    
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

heap_node remove_min (heap_node *heap, int32_t size) {
    heap_node rv = heap[1];
    
    heap[1] = heap[size];
    
    --size;
    
    shift_down(heap, size, 1);
    
    return rv;
}

void enqueue (heap_node node, PQ *q) {
    insert(node, q->heap, q->size);
    
    ++q->size;
}

heap_node dequeue (PQ *q) {
    heap_node rv = remove_min(q->heap, q->size);
    
    --q->size;
    
    return rv;
}

void init_queue (PQ *q, int32_t n) {
    q->size = 0;
    q->heap = (heap_node*) malloc(sizeof(heap_node) * (n+1));
}