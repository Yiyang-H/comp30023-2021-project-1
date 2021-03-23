#ifndef PQUEUE_H
#define PQUEUE_H

#include <stdbool.h>

// Defines a priority queue data structure
typedef struct pqueue Pqueue;
typedef struct node Node;
typedef struct process Process;

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
    Process* parent_process;
    unsigned int subprocess_id;
    unsigned int time_remain;
    unsigned int time_finished;
}Process;

typedef struct node {
    Process *process;
    Node *prev;
    Node *next;
}Node;



Pqueue *new_queue();

void free_queue(Pqueue *queue);

void push(Pqueue *queue, Process *process);

Process *pop(Pqueue *queue);

bool compare_process(Process *p1, Process *p2);

Process *create_subprocess(Process *parent, int sub_id, int subprocess_execution);

#endif