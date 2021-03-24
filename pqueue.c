#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "pqueue.h"



void free_node(Node *node);
Node *new_node();
bool compare_process(Process *p1, Process *p2);

Pqueue *new_queue() {
    Pqueue *queue = malloc(sizeof(*queue));
    assert(queue);

    queue->start = NULL;
    queue->end = NULL;
    queue->size = 0;

    return queue;
}

void free_queue(Pqueue *queue) {
    assert(queue != NULL);

    Node *node = queue->start;
    Node *next;

    while(node) {
        next = node->next;
        free_node(node);
        node = next;
    }

    free(queue);
}

// Push a process onto the queue according to their remaining time
void push(Pqueue *queue, Process *process) {
    assert(queue != NULL);
    
    Node *node = new_node();
    node->process = process;

    if(queue->size == 0) {
        queue->start = node;
        queue->end = node;
        node->prev = NULL;
        node->next = NULL;
        queue->size++;
        return;
    }

    Node *cur = queue->start;
    bool end_of_queue = false;
    // Check if the new process is the start of the queue
    if(!compare_process(cur->process,process)) {
        queue->start = node;
        node->next = cur;
        cur->prev = node;
    }else{
        // If the queue has at least one item, find a suitable position
        while(compare_process(cur->process,process)) {
            if(queue->end == cur) {
                end_of_queue = true;
                break;
            }
            cur = cur->next;
        }
        if(end_of_queue) {
            cur->next = node;
            node->prev = cur;
            node->next = NULL;
        }else {
            cur = cur->prev;
            node->next = cur->next;
            node->prev = cur;
            cur->next = node;
            node->next->prev = node;
        }

    }

    if(end_of_queue) {
        queue->end = node;
    }

    // increment the size of the queue
    queue->size++;
}

// Pop the start of the queue(lowest remaining time)
Process *pop(Pqueue *queue) {
    
    Node *node = queue->start;
    Process *p = node->process;

    if(queue->size == 1) {
        queue->start = NULL;
        queue->end = NULL;
    }else {
        queue->start = node->next;
        queue->start->prev = NULL;
    }

    free_node(node);
    
    queue->size--;
    return p;
}

void free_node(Node *node) {
    free(node);
}

Node *new_node() {
    Node *node = malloc(sizeof(*node));
    assert(node);

    return node;
}

// Compare two process and return true if p1 should be in front of p2
bool compare_process(Process *p1, Process *p2){
    if(p1->time_remain < p2->time_remain) {
        return true;
    }
    if(p1->time_remain > p2->time_remain) {
        return false;
    }
    return p1->process_id < p2->process_id;
}
