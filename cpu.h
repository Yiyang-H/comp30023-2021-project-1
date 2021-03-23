#ifndef CPU_H
#define CPU_H

#include "pqueue.h"

typedef struct cpu CPU;

typedef struct cpu {
    int cpu_id;
    // unsigned int total_time_remain;
    Process *cur_process;
    Pqueue *queue;
}CPU;

// void new_process(CPU* cpu, Process* process);

// Execute the process with shortest time remain
// Process* execute_process(CPU* cpu);

unsigned int total_time_remain(CPU* cpu);




#endif