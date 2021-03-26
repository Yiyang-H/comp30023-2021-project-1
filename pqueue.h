#ifndef PQUEUE_H
#define PQUEUE_H

#include <stdbool.h>

// Defines a priority queue data structure
typedef struct pqueue Pqueue;
typedef struct node Node;
typedef struct process process_t;

typedef struct pqueue {
    Node *start;
    Node *end;
    int size;
}Pqueue;

typedef struct process {
    unsigned int time_arrived;
    unsigned int process_id;
    unsigned int execution_time;
    bool parallelisable;
    process_t* parent_process;
    unsigned int subprocess_id;
    unsigned int num_subprocess;
    unsigned int time_remain;
    unsigned int time_finished;
}process_t;

typedef struct node {
    process_t *process;
    Node *prev;
    Node *next;
}Node;

Pqueue *new_queue();

void free_queue(Pqueue *queue);

void push(Pqueue *queue, process_t *process);

process_t *pop(Pqueue *queue);

process_t *pop_longest(Pqueue *queue);

bool compare_process(process_t *p1, process_t *p2);

process_t *create_subprocess(process_t *parent, int sub_id, int subprocess_execution);

#endif