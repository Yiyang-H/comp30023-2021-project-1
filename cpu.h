#ifndef CPU_H
#define CPU_H

#include "pqueue.h"

typedef struct cpu CPU;

typedef struct cpu {
    int cpu_id;
    unsigned int total_time_remain;
    Process *cur_process;
    Pqueue *queue;
}CPU;

#endif