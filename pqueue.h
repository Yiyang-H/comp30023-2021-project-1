#ifndef PQUEUE_H
#define PQUEUE_H

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
    int process_id;
    int time_remain;
}Process;

typedef struct node {
    Process process;
    Node *next;
}Node;



Pqueue *new_queue();

void free_queue(Pqueue *queue);

void push(Pqueue *queue, Process process);

Process pop(Pqueue *queue);

#endif