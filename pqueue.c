#include <stdlib.h>
#include <assert.h>

#include "pqueue.h"



void free_node(Node *node);
Node *new_node();

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
void push(Pqueue *queue, Process process) {
    assert(queue != NULL);
    
    Node *node = new_node();
    node->process = process;

    if(queue->size == 0) {
        queue->start = node;
        queue->end = node;
        node->next = NULL;
        return;
    }

    Node *cur = queue->start;

    // If the queue has at least one item, find a suitable position
    while(cur->process.time_remain < process.time_remain) {
        cur = cur->next;
    }
    // Not considering the end yet

    node->next = cur;
    cur = node; // Is this correct?

    // increment the size of the queue
    queue->size++;
}

// Pop the start of the queue(lowest remaining time)
Process pop(Pqueue *queue) {
    queue->size--;
    return queue->start->process;
}

void free_node(Node *node) {
    free(node);
}

Node *new_node() {
    Node *node = malloc(sizeof(*node));
    assert(node);

    return node;
}

